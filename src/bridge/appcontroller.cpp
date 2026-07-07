#include "appcontroller.h"
#include "transport/transportconfig.h"
#include "transport/serialtransport.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_transport(new TransportController(this))
    , m_dataProcessor(new DataProcessor(this))
    , m_quickSendModel(new QuickSendModel(20, this))
    , m_autoSendTimer(new QTimer(this))
    , m_fileSendTimer(new QTimer(this))
{
    // 连接通信控制器信号
    connect(m_transport, &TransportController::dataReceived,
            this, &AppController::onDataReceived);
    connect(m_transport, &TransportController::connectedChanged,
            this, &AppController::onConnectedChanged);
    connect(m_transport, &TransportController::errorOccurred,
            this, &AppController::onErrorOccurred);

    // 连接定时器信号
    connect(m_autoSendTimer, &QTimer::timeout,
            this, &AppController::onAutoSendTimer);
    connect(m_fileSendTimer, &QTimer::timeout,
            this, &AppController::onFileSendTimer);

    // 加载设置
    SettingsManager::instance()->load();

    // 加载快速发送条目
    QVariantList entries = SettingsManager::instance()->quickSendEntries();
    if (!entries.isEmpty()) {
        m_quickSendModel->loadFromVariantList(entries);
    }

    // 初始化串口通信
    switchToTransport(0);
}

AppController::~AppController()
{
    // 保存设置
    SettingsManager *s = SettingsManager::instance();
    s->setQuickSendEntries(m_quickSendModel->toVariantList());
    s->save();
}

bool AppController::isConnected() const
{
    return m_transport->isConnected();
}

void AppController::setConnectionType(int type)
{
    if (m_connectionType != type) {
        m_connectionType = type;
        switchToTransport(type);
        SettingsManager::instance()->setConnectionType(type);
        emit connectionTypeChanged();
        clearStatistics();
    }
}

void AppController::sendData(const QString &text, bool hexMode)
{
    if (text.isEmpty()) {
        emit errorMessage(tr("发送框为空"));
        stopAutoSend();
        return;
    }

    // 缓存发送内容（用于自动发送）
    m_lastSendText = text;
    m_lastSendHexMode = hexMode;

    // 根据模式转换数据
    QByteArray sendData;
    if (hexMode) {
        sendData = DataProcessor::fromHexString(text);
    } else {
        sendData = text.toLocal8Bit();
    }

    // 发送数据
    bool ok = m_transport->sendData(sendData);
    if (ok) {
        // 更新发送字节计数
        m_sentBytes += sendData.size();
        emit sentBytesChanged();

        // 显示发送的数据
        bool hexShow = SettingsManager::instance()->hexShow();
        bool addTime = SettingsManager::instance()->addTimestamp();
        QString displayStr = hexShow ? DataProcessor::toHexString(sendData)
                                     : DataProcessor::toLocalString(sendData);
        if (addTime) {
            displayStr = DataProcessor::appendTimestamp(displayStr, true);
        }
        appendToDisplay(displayStr);
    } else {
        stopAutoSend();
    }

    emit sendStatusChanged(ok);
}

void AppController::sendQuickSendData(int row)
{
    QString text = m_quickSendModel->entryData(row);
    if (text.isEmpty())
        return;

    bool hexMode = m_quickSendModel->entryHex(row);

    // 根据模式转换数据
    QByteArray sendData;
    if (hexMode) {
        sendData = DataProcessor::fromHexString(text);
    } else {
        sendData = text.toLocal8Bit();
    }

    // 发送数据
    bool ok = m_transport->sendData(sendData);
    if (ok) {
        // 显示发送的数据
        bool hexShow = SettingsManager::instance()->hexShow();
        bool addTime = SettingsManager::instance()->addTimestamp();
        QString displayStr = hexShow ? DataProcessor::toHexString(sendData)
                                     : DataProcessor::toLocalString(sendData);
        if (addTime) {
            displayStr = DataProcessor::appendTimestamp(displayStr, true);
        }
        appendToDisplay(displayStr);
    }
    emit sendStatusChanged(ok);
}

void AppController::clearDisplay()
{
    m_displayText.clear();
    emit displayTextChanged(m_displayText);
    clearStatistics();
}

void AppController::saveDataToFile(const QUrl &url)
{
    QString path = url.toLocalFile();
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorMessage(tr("文件保存失败"));
        return;
    }

    file.write(m_displayText.toUtf8());
    file.close();
}

void AppController::loadAndSendFile(const QUrl &url)
{
    QString path = url.toLocalFile();
    if (path.isEmpty()) {
        emit errorMessage(tr("未选择文件"));
        return;
    }

    // 关闭之前正在发送的文件
    if (m_sendingFile) {
        m_sendingFile->close();
        delete m_sendingFile;
        m_sendingFile = nullptr;
    }

    // 打开新文件
    m_sendingFile = new QFile(path, this);
    if (!m_sendingFile->open(QIODevice::ReadOnly)) {
        emit errorMessage(tr("文件打开失败"));
        delete m_sendingFile;
        m_sendingFile = nullptr;
        return;
    }

    // 初始化文件发送状态
    QFileInfo info(path);
    m_fileSize = info.size();
    m_fileSentSize = 0;

    // 显示发送开始提示
    QString msg = tr("[文件发送开始] %1 (%2 字节)").arg(info.fileName()).arg(m_fileSize);
    bool addTime = SettingsManager::instance()->addTimestamp();
    if (addTime) {
        msg = DataProcessor::appendTimestamp(msg, true);
    }
    appendToDisplay(msg);

    // 启动异步文件发送（每10ms发送1KB）
    emit fileProgressChanged(0);
    m_fileSendTimer->start(10);
}

void AppController::refreshPorts()
{
    m_transport->refreshPorts();
}

void AppController::addManualPort(const QString &portName)
{
    if (portName.isEmpty()) {
        emit errorMessage(tr("端口不能为空"));
        return;
    }

    // 检查端口是否重复
    QStringList ports = SettingsManager::instance()->manualPorts();
    if (ports.contains(portName)) {
        emit errorMessage(tr("端口不能重复"));
        return;
    }

    // 添加端口并排序
    ports.append(portName);
    ports.sort(Qt::CaseInsensitive);
    SettingsManager::instance()->setManualPorts(ports);

    // 刷新端口列表
    if (auto *serial = dynamic_cast<SerialTransport *>(m_transport->currentTransport())) {
        serial->refreshPorts();
    }
}

bool AppController::openSerialPort(const QString &portName, int baudRate)
{
    // 配置串口参数
    SerialConfig config;
    config.portName = portName;
    config.baudRate = baudRate;
    config.dataBits = static_cast<QSerialPort::DataBits>(SettingsManager::instance()->serialDataBits());
    config.stopBits = static_cast<QSerialPort::StopBits>(SettingsManager::instance()->serialStopBits());
    config.parity = static_cast<QSerialPort::Parity>(SettingsManager::instance()->serialParity());
    config.flowControl = static_cast<QSerialPort::FlowControl>(SettingsManager::instance()->serialFlowControl());

    // 切换到串口通信
    m_connectionType = 0;
    m_transport->switchTransport(TransportType::Serial, config);
    bool ok = m_transport->currentTransport()->open();
    if (ok) {
        emit connectedChanged();
        emit connectionTypeChanged();
    }
    return ok;
}

bool AppController::startTcpServer(int port)
{
    // 配置TCP服务器参数
    TcpServerConfig config;
    config.listenPort = port;

    // 切换到TCP服务器通信
    m_connectionType = 1;
    m_transport->switchTransport(TransportType::TcpServer, config);
    bool ok = m_transport->currentTransport()->open();
    if (ok) {
        SettingsManager::instance()->setTcpServerPort(QString::number(port));
        emit connectedChanged();
        emit connectionTypeChanged();
    }
    return ok;
}

bool AppController::connectTcpClient(const QString &host, int port)
{
    // 配置TCP客户端参数
    TcpClientConfig config;
    config.host = host;
    config.port = port;

    // 切换到TCP客户端通信
    m_connectionType = 2;
    m_transport->switchTransport(TransportType::TcpClient, config);
    bool ok = m_transport->currentTransport()->open();
    if (ok) {
        SettingsManager::instance()->setTcpClientIP(host);
        SettingsManager::instance()->setTcpClientPort(QString::number(port));
        emit connectedChanged();
        emit connectionTypeChanged();
    }
    return ok;
}

bool AppController::bindUdp(int localPort)
{
    // 配置UDP参数
    UdpConfig config;
    config.localPort = localPort;
    config.peerHost = SettingsManager::instance()->udpPeerIP();
    config.peerPort = SettingsManager::instance()->udpPeerPort().toUInt();

    // 切换到UDP通信
    m_connectionType = 3;
    m_transport->switchTransport(TransportType::Udp, config);
    bool ok = m_transport->currentTransport()->open();
    if (ok) {
        SettingsManager::instance()->setUdpLocalPort(QString::number(localPort));
        emit connectedChanged();
        emit connectionTypeChanged();
    }
    return ok;
}

void AppController::closeConnection()
{
    m_transport->closeCurrent();
    emit connectedChanged();
    clearStatistics();
}

void AppController::startAutoSend(int intervalMs)
{
    if (intervalMs <= 0) {
        emit errorMessage(tr("请输入时间"));
        return;
    }

    // 缓存当前发送内容
    m_autoSendText = m_lastSendText;
    m_autoSendHexMode = m_lastSendHexMode;

    // 启动定时器
    m_autoSendTimer->start(intervalMs);
}

void AppController::stopAutoSend()
{
    m_autoSendTimer->stop();
}

void AppController::clearStatistics()
{
    m_sentBytes = 0;
    m_receivedBytes = 0;
    emit sentBytesChanged();
    emit receivedBytesChanged();
}

void AppController::onDataReceived(const QByteArray &data)
{
    // 更新接收字节计数
    m_receivedBytes += data.size();
    emit receivedBytesChanged();

    // 显示接收到的数据
    bool hexShow = SettingsManager::instance()->hexShow();
    bool addTime = SettingsManager::instance()->addTimestamp();

    QString displayStr = hexShow ? DataProcessor::toHexString(data)
                                 : DataProcessor::toLocalString(data);

    if (addTime) {
        displayStr = DataProcessor::appendTimestamp(displayStr, false);
    }

    appendToDisplay(displayStr);
}

void AppController::onConnectedChanged()
{
    emit connectedChanged();
}

void AppController::onErrorOccurred(const QString &message)
{
    emit errorMessage(message);
}

void AppController::onAutoSendTimer()
{
    // 定时发送缓存的数据
    if (!m_autoSendText.isEmpty()) {
        sendData(m_autoSendText, m_autoSendHexMode);
    }
}

void AppController::onFileSendTimer()
{
    sendNextFileChunk();
}

void AppController::appendToDisplay(const QString &text)
{
    if (SettingsManager::instance()->addTimestamp()) {
        m_displayText += text + "\n";
    } else {
        m_displayText += text;
    }
    emit displayTextChanged(m_displayText);
}

void AppController::switchToTransport(int typeIndex)
{
    // 转换连接类型枚举
    TransportType type;
    switch (typeIndex) {
    case 0: type = TransportType::Serial; break;
    case 1: type = TransportType::TcpServer; break;
    case 2: type = TransportType::TcpClient; break;
    case 3: type = TransportType::Udp; break;
    default: type = TransportType::Serial; break;
    }

    // 根据类型创建配置
    TransportConfig config;
    SettingsManager *s = SettingsManager::instance();

    switch (type) {
    case TransportType::Serial: {
        SerialConfig serialConfig;
        serialConfig.baudRate = 115200; // 默认值，实际由UI设置
        serialConfig.dataBits = static_cast<QSerialPort::DataBits>(s->serialDataBits());
        serialConfig.stopBits = static_cast<QSerialPort::StopBits>(s->serialStopBits());
        serialConfig.parity = static_cast<QSerialPort::Parity>(s->serialParity());
        serialConfig.flowControl = static_cast<QSerialPort::FlowControl>(s->serialFlowControl());
        config = serialConfig;
        break;
    }
    case TransportType::TcpServer: {
        TcpServerConfig tcpConfig;
        tcpConfig.listenPort = s->tcpServerPort().toUInt();
        config = tcpConfig;
        break;
    }
    case TransportType::TcpClient: {
        TcpClientConfig tcpConfig;
        tcpConfig.host = s->tcpClientIP();
        tcpConfig.port = s->tcpClientPort().toUInt();
        config = tcpConfig;
        break;
    }
    case TransportType::Udp: {
        UdpConfig udpConfig;
        udpConfig.localPort = s->udpLocalPort().toUInt();
        udpConfig.peerHost = s->udpPeerIP();
        udpConfig.peerPort = s->udpPeerPort().toUInt();
        config = udpConfig;
        break;
    }
    }

    // 切换通信对象
    m_transport->switchTransport(type, config);
}

void AppController::sendNextFileChunk()
{
    if (!m_sendingFile || !m_sendingFile->isOpen()) {
        m_fileSendTimer->stop();
        return;
    }

    // 每次发送1KB数据
    const int chunkSize = 1024;
    char buf[chunkSize];

    qint64 l = m_sendingFile->read(buf, chunkSize);
    if (l <= 0) {
        // 文件发送完成
        m_fileSendTimer->stop();
        m_sendingFile->close();
        delete m_sendingFile;
        m_sendingFile = nullptr;

        QString msg = tr("[文件发送完成] 共发送 %1 字节").arg(m_fileSentSize);
        bool addTime = SettingsManager::instance()->addTimestamp();
        if (addTime) {
            msg = DataProcessor::appendTimestamp(msg, true);
        }
        appendToDisplay(msg);
        emit fileProgressChanged(100);
        return;
    }

    // 发送数据块
    QByteArray data(buf, l);
    bool ok = m_transport->sendData(data);
    if (!ok) {
        // 发送失败
        m_fileSendTimer->stop();
        m_sendingFile->close();
        delete m_sendingFile;
        m_sendingFile = nullptr;

        QString msg = tr("[文件发送失败] 已发送 %1 / %2 字节").arg(m_fileSentSize).arg(m_fileSize);
        bool addTime = SettingsManager::instance()->addTimestamp();
        if (addTime) {
            msg = DataProcessor::appendTimestamp(msg, true);
        }
        appendToDisplay(msg);
        return;
    }

    // 更新发送字节计数
    m_sentBytes += data.size();
    emit sentBytesChanged();

    // 更新进度
    m_fileSentSize += l;
    int pos = static_cast<int>(m_fileSentSize * 100 / m_fileSize);
    emit fileProgressChanged(pos);
}
