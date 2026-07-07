#include "serialtransport.h"
#include "settings/settingsmanager.h"

SerialTransport::SerialTransport(QObject *parent)
    : AbstractTransport(parent)
    , m_serial(new QSerialPort(this))
{
    // 连接数据接收信号
    connect(m_serial, &QSerialPort::readyRead, this, &SerialTransport::onReadyRead);
    refreshPorts();
}

SerialTransport::~SerialTransport()
{
    close();
}

void SerialTransport::setConfig(const SerialConfig &config)
{
    m_config = config;
}

bool SerialTransport::open()
{
    // 配置串口参数
    m_serial->setPortName(m_config.portName);
    m_serial->setBaudRate(m_config.baudRate);
    m_serial->setDataBits(m_config.dataBits);
    m_serial->setStopBits(m_config.stopBits);
    m_serial->setParity(m_config.parity);
    m_serial->setFlowControl(m_config.flowControl);

    // 打开串口
    bool ok = m_serial->open(QIODevice::ReadWrite);
    if (ok) {
        emit connectedChanged();
    } else {
        emit errorOccurred(m_serial->errorString());
    }
    return ok;
}

void SerialTransport::close()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        emit connectedChanged();
    }
}

bool SerialTransport::isConnected() const
{
    return m_serial->isOpen();
}

bool SerialTransport::sendData(const QByteArray &data)
{
    if (!m_serial->isOpen()) {
        emit errorOccurred(tr("串口未打开"));
        return false;
    }

    qint64 len = m_serial->write(data);
    return len == data.size();
}

QString SerialTransport::errorString() const
{
    return m_serial->errorString();
}

QStringList SerialTransport::availablePorts() const
{
    return m_availablePorts;
}

void SerialTransport::refreshPorts()
{
    QStringList systemPorts = enumeratePorts();
    QStringList manualPorts = SettingsManager::instance()->manualPorts();

    // 系统端口升序排序
    systemPorts.sort(Qt::CaseInsensitive);

    // 手动添加的端口在前，系统端口在后，去除重复
    m_availablePorts.clear();

    // 先添加手动端口（已排序）
    foreach (const QString &port, manualPorts) {
        if (!m_availablePorts.contains(port)) {
            m_availablePorts.append(port);
        }
    }

    // 再添加系统端口（跳过已添加的手动端口）
    foreach (const QString &port, systemPorts) {
        if (!m_availablePorts.contains(port)) {
            m_availablePorts.append(port);
        }
    }

    emit availablePortsChanged();
}

QStringList SerialTransport::enumeratePorts()
{
    // 枚举系统所有可用串口
    QStringList ports;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ports.append(info.portName());
    }
    return ports;
}

void SerialTransport::onReadyRead()
{
    // 读取所有可用数据并发送接收信号
    QByteArray data = m_serial->readAll();
    emit dataReceived(data);
}
