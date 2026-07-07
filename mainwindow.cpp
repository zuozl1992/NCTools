#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QMessageBox>
#include <QDateTime>
#include <QRegularExpression>
#include "serialmoreset.h"
#include <QDir>
#include <QTimerEvent>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QSettings>
#include <QStandardPaths>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpSocket(new QUdpSocket(this))
    , tcpClientSocket(new QTcpSocket(this))
    , tcpServer(new QTcpServer(this))
    , serial(new QSerialPort(this))
    , expPanel(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("MySSCOM - 串口网络调试工具"));

    QMenu *helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    QAction *aboutAction = helpMenu->addAction(tr("关于(&A)"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::on_actionAbout_triggered);

    initPort();
    initExpPanel();
    loadSettings();
    connect(udpSocket, &QUdpSocket::readyRead,
            this, &MainWindow::udpReadyReadSlot);
    connect(tcpClientSocket, &QTcpSocket::readyRead,
            this, &MainWindow::tcpClientReadyReadSlot);
    connect(tcpClientSocket, &QTcpSocket::disconnected,
            this, &MainWindow::tcpClientDisconnectedSlot);
    connect(tcpServer, &QTcpServer::newConnection,
            this, &MainWindow::tcpServerNewConnectionSlot);
    connect(serial, &QSerialPort::readyRead,
            this, &MainWindow::serialReadyReadSlot);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::udpReadyReadSlot()
{
    QByteArray data;
    char buf[2048];
    qint64 len = udpSocket->readDatagram(buf, 2048);
    data.append(buf, len);
    showDataToUI(data);
}

void MainWindow::tcpClientReadyReadSlot()
{
    QByteArray data;
    data = tcpClientSocket->readAll();
    showDataToUI(data);
}

void MainWindow::tcpClientDisconnectedSlot()
{
    ui->leTcpClientServerIP->setEnabled(true);
    ui->leTcpClientServerPort->setEnabled(true);
    ui->btnTcpClientConnect->setText(tr("连接服务器"));
    showDataToUI(tr("服务器已断开连接\n").toLocal8Bit());
}

void MainWindow::tcpServerNewConnectionSlot()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    tcpServerSocketList.append(socket);
    connect(socket, &QTcpSocket::readyRead,
            this, &MainWindow::tcpServerSocketReadyReadSlot);
    connect(socket, &QTcpSocket::disconnected,
            this, &MainWindow::tcpServerSocketDisconnectedSlot);
    QHostAddress ip = socket->peerAddress();
    quint16 port = socket->peerPort();
    bool ok = false;
    quint32 ipU32 = ip.toIPv4Address(&ok);
    QString ipStr;
    if(ok){
        quint8 ia[4];
        ia[0] = ipU32;
        ia[1] = ipU32 >> 8;
        ia[2] = ipU32 >> 16;
        ia[3] = ipU32 >> 24;
        ipStr = QString("%1.%2.%3.%4")
                    .arg(ia[3]).arg(ia[2]).arg(ia[1]).arg(ia[0]);
    }
    else
        ipStr = ip.toString();
    socket->setProperty("ip_str", ipStr);
    socket->setProperty("port_str", QString("%1").arg(port));
}

void MainWindow::tcpServerSocketReadyReadSlot()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data;
    data = socket->readAll();
    showDataToUI(data);
}

void MainWindow::tcpServerSocketDisconnectedSlot()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    disconnect(socket, nullptr, nullptr, nullptr);
    tcpServerSocketList.removeOne(socket);
}

void MainWindow::serialReadyReadSlot()
{
    QByteArray data;
    data = serial->readAll();
    showDataToUI(data);
}

void MainWindow::initPort()
{
    ui->cbName->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->cbName->addItem(info.portName());
    }
    ui->cbName->addItem("TCP Server");
    ui->cbName->addItem("TCP Client");
    ui->cbName->addItem("UDP");
    if(ui->cbName->count() > 3)
        ui->swMain->setCurrentIndex(0);
    else
        ui->swMain->setCurrentIndex(1);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::initExpPanel()
{
    const int rowCount = 20;

    expPanel = new QWidget(this);
    expPanel->setFixedWidth(350);
    expPanel->setVisible(false);

    QVBoxLayout *panelLayout = new QVBoxLayout(expPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(2);

    QWidget *headerWidget = new QWidget(expPanel);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(2, 2, 17, 2);
    headerLayout->addWidget(new QLabel(tr("#"), headerWidget), 1);
    headerLayout->addWidget(new QLabel(tr("数据"), headerWidget), 5);
    headerLayout->addWidget(new QLabel(tr("Hex"), headerWidget), 1);
    headerLayout->addWidget(new QLabel(tr("操作"), headerWidget), 1);
    panelLayout->addWidget(headerWidget);

    QScrollArea *scrollArea = new QScrollArea(expPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea{border: none;}");

    QWidget *scrollContent = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(scrollContent);
    gridLayout->setContentsMargins(2, 2, 2, 2);
    gridLayout->setSpacing(2);

    for(int i = 0; i < rowCount; i++){
        QLabel *label = new QLabel(QString("%1").arg(i + 1, 2, 10, QChar('0')), scrollContent);
        label->setFixedWidth(25);
        label->setAlignment(Qt::AlignCenter);

        QLineEdit *le = new QLineEdit(scrollContent);
        le->setPlaceholderText(tr("输入数据%1").arg(i + 1));

        QCheckBox *cb = new QCheckBox(tr("Hex"), scrollContent);

        QPushButton *btn = new QPushButton(tr("发送"), scrollContent);
        btn->setFixedWidth(50);

        gridLayout->addWidget(label, i, 0);
        gridLayout->addWidget(le, i, 1);
        gridLayout->addWidget(cb, i, 2);
        gridLayout->addWidget(btn, i, 3);

        expLineEdits.append(le);
        expHexChecks.append(cb);

        connect(btn, &QPushButton::clicked, this, [this, i](){
            QString text = expLineEdits[i]->text();
            if(text.isEmpty())
                return;
            QByteArray sendData;
            if(expHexChecks[i]->isChecked()){
                sendData = QByteArray::fromHex(
                    text.remove(QRegularExpression("\\s")).toLocal8Bit());
            }
            else{
                sendData = text.toLocal8Bit();
            }
            bool re = autoSendData(sendData);
            if(re)
                showDataToUI(sendData, 1);
        });
    }

    gridLayout->setRowStretch(rowCount, 1);
    scrollArea->setWidget(scrollContent);
    panelLayout->addWidget(scrollArea);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(6);

    QWidget *originalContent = new QWidget(this);
    originalContent->setLayout(ui->centralwidget->layout());

    mainLayout->addWidget(originalContent);
    mainLayout->addWidget(expPanel);

    ui->centralwidget->setLayout(mainLayout);
}

void MainWindow::saveSettings()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                         + "/MySSCOM";
    QDir().mkpath(configPath);
    QSettings settings(configPath + "/config.ini", QSettings::IniFormat);

    settings.beginGroup("Serial");
    settings.setValue("baudrate", ui->cbSerialBaudrate->currentIndex());
    settings.setValue("databits", (int)serial->dataBits());
    settings.setValue("stopbits", (int)serial->stopBits());
    settings.setValue("parity", (int)serial->parity());
    settings.setValue("flowcontrol", (int)serial->flowControl());
    QStringList manualPorts;
    for(int i = 0; i < ui->cbName->count() - 3; i++){
        bool isSystemPort = false;
        foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            if(info.portName() == ui->cbName->itemText(i)){
                isSystemPort = true;
                break;
            }
        }
        if(!isSystemPort)
            manualPorts.append(ui->cbName->itemText(i));
    }
    settings.setValue("manualPorts", manualPorts);
    settings.endGroup();

    settings.beginGroup("TCPServer");
    settings.setValue("port", ui->leTcpServerPort->text());
    settings.endGroup();

    settings.beginGroup("TCPClient");
    settings.setValue("ip", ui->leTcpClientServerIP->text());
    settings.setValue("port", ui->leTcpClientServerPort->text());
    settings.endGroup();

    settings.beginGroup("UDP");
    settings.setValue("localPort", ui->leUdpLocalPort->text());
    settings.setValue("peerIP", ui->leUdpPeerIP->text());
    settings.setValue("peerPort", ui->leUdpPeerPort->text());
    settings.endGroup();

    settings.beginGroup("Display");
    settings.setValue("hexShow", ui->cbHexShow->isChecked());
    settings.setValue("hexSend", ui->cbHexSend->isChecked());
    settings.setValue("autoClearSend", ui->cbAutoClearSend->isChecked());
    settings.setValue("addTime", ui->cbAddTime->isChecked());
    settings.setValue("sendAuto", ui->cbSendAuto->isChecked());
    settings.setValue("autoSendTime", ui->leAutoSendTime->text());
    settings.endGroup();

    settings.beginGroup("General");
    settings.setValue("connectionType", ui->swMain->currentIndex());
    settings.endGroup();

    settings.beginGroup("ExpPanel");
    settings.setValue("rowCount", expLineEdits.size());
    for(int i = 0; i < expLineEdits.size(); i++){
        settings.setValue(QString("data_%1").arg(i), expLineEdits[i]->text());
        settings.setValue(QString("hex_%1").arg(i), expHexChecks[i]->isChecked());
    }
    settings.endGroup();
}

void MainWindow::loadSettings()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                         + "/MySSCOM/config.ini";
    if(!QFile::exists(configPath))
        return;
    QSettings settings(configPath, QSettings::IniFormat);

    settings.beginGroup("Serial");
    int baudIdx = settings.value("baudrate", 2).toInt();
    if(baudIdx >= 0 && baudIdx < ui->cbSerialBaudrate->count())
        ui->cbSerialBaudrate->setCurrentIndex(baudIdx);
    int dataBits = settings.value("databits", (int)QSerialPort::Data8).toInt();
    int stopBits = settings.value("stopbits", (int)QSerialPort::OneStop).toInt();
    int parity = settings.value("parity", (int)QSerialPort::NoParity).toInt();
    int flow = settings.value("flowcontrol", (int)QSerialPort::NoFlowControl).toInt();
    serial->setDataBits((QSerialPort::DataBits)dataBits);
    serial->setStopBits((QSerialPort::StopBits)stopBits);
    serial->setParity((QSerialPort::Parity)parity);
    serial->setFlowControl((QSerialPort::FlowControl)flow);
    QStringList manualPorts = settings.value("manualPorts").toStringList();
    foreach(const QString &name, manualPorts){
        bool exists = false;
        for(int i = 0; i < ui->cbName->count(); i++){
            if(ui->cbName->itemText(i) == name){
                exists = true;
                break;
            }
        }
        if(!exists)
            ui->cbName->insertItem(ui->cbName->count() - 3, name);
    }
    settings.endGroup();

    settings.beginGroup("TCPServer");
    ui->leTcpServerPort->setText(settings.value("port", "10000").toString());
    settings.endGroup();

    settings.beginGroup("TCPClient");
    ui->leTcpClientServerIP->setText(settings.value("ip", "127.0.0.1").toString());
    ui->leTcpClientServerPort->setText(settings.value("port", "10000").toString());
    settings.endGroup();

    settings.beginGroup("UDP");
    ui->leUdpLocalPort->setText(settings.value("localPort", "10001").toString());
    ui->leUdpPeerIP->setText(settings.value("peerIP", "127.0.0.1").toString());
    ui->leUdpPeerPort->setText(settings.value("peerPort", "10002").toString());
    settings.endGroup();

    settings.beginGroup("Display");
    ui->cbHexShow->setChecked(settings.value("hexShow", false).toBool());
    ui->cbHexSend->setChecked(settings.value("hexSend", false).toBool());
    ui->cbAutoClearSend->setChecked(settings.value("autoClearSend", false).toBool());
    ui->cbAddTime->setChecked(settings.value("addTime", false).toBool());
    ui->leAutoSendTime->setText(settings.value("autoSendTime", "1000").toString());
    ui->cbSendAuto->setChecked(settings.value("sendAuto", false).toBool());
    settings.endGroup();

    settings.beginGroup("General");
    int connType = settings.value("connectionType", 0).toInt();
    if(connType >= 0 && connType < ui->swMain->count()){
        ui->swMain->setCurrentIndex(connType);
        if(connType == 0){
            int idx = ui->cbName->count() > 3 ? 0 : -1;
            if(idx >= 0) ui->cbName->setCurrentIndex(idx);
        }
        else if(connType == 1)
            ui->cbName->setCurrentIndex(ui->cbName->count() - 3);
        else if(connType == 2)
            ui->cbName->setCurrentIndex(ui->cbName->count() - 2);
        else if(connType == 3)
            ui->cbName->setCurrentIndex(ui->cbName->count() - 1);
    }
    settings.endGroup();

    settings.beginGroup("ExpPanel");
    int count = settings.value("rowCount", 0).toInt();
    for(int i = 0; i < count && i < expLineEdits.size(); i++){
        expLineEdits[i]->setText(settings.value(QString("data_%1").arg(i)).toString());
        expHexChecks[i]->setChecked(settings.value(QString("hex_%1").arg(i)).toBool());
    }
    settings.endGroup();
}

void MainWindow::on_btnExp_clicked()
{
    bool visible = !expPanel->isVisible();
    expPanel->setVisible(visible);
    ui->btnExp->setText(visible ? tr("收起") : tr("扩展"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("关于 MySSCOM"));
    aboutBox.setIconPixmap(QPixmap(":/control.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    aboutBox.setText(tr("<h3>MySSCOM</h3>"
                       "<p>版本 1.0</p>"
                       "<p>串口网络调试工具</p>"
                       "<p>支持串口、TCP Server、TCP Client、UDP 通信</p>"
                       "<p>Copyright &copy; 2024</p>"));
    aboutBox.exec();
}

bool MainWindow::sendUseUdp(const QByteArray &data)
{
    QString ip = ui->leUdpPeerIP->text();
    quint16 port = ui->leUdpPeerPort->text().toUInt();
    if(ip.isEmpty() || port == 0){
        execWarnMsg(tr("请输入对方ip和端口"));
        return false;
    }
    qint64 len = udpSocket->writeDatagram(
        data,
        QHostAddress(ip),
        port);
    return len == data.size() ? true : false;
}

bool MainWindow::sendUseTcpClient(const QByteArray &data)
{
    if(tcpClientSocket->state() != QTcpSocket::ConnectedState){
        execWarnMsg(tr("未连接服务器"));
        return false;
    }
    qint64 len = tcpClientSocket->write(data);
    return len == data.size() ? true : false;
}

bool MainWindow::sendUseTcpServer(const QByteArray &data)
{
    bool re = true;
    foreach (QTcpSocket *s, tcpServerSocketList) {
        quint64 len = s->write(data);
        if(len != data.size())
            re = false;
    }
    return re;
}

bool MainWindow::sendUseSerial(const QByteArray &data)
{
    if(!serial->isOpen()){
        execWarnMsg(tr("串口未打开"));
        return false;
    }
    qint64 len = serial->write(data);
    return len == data.size() ? true : false;
}

void MainWindow::execWarnMsg(QString msg)
{
    QMessageBox msgBox;
    msgBox.addButton(tr("确定"), QMessageBox::YesRole);
    msgBox.setWindowTitle(tr("警告"));
    msgBox.setText(msg);
    msgBox.exec();
}

void MainWindow::showDataToUI(const QByteArray &data, int dir)
{
    QString str;
    if(ui->cbHexShow->isChecked()){
        str = data.toHex(' ').toUpper();
    }
    else{
        str = QString::fromLocal8Bit(data);
    }
    if(ui->cbAddTime->isChecked()){
        ui->tbReadData->append(
            tr("[%1]%3:%2")
                .arg(QDateTime::currentDateTime()
                         .toString("yyyy-MM-dd hh:mm:ss.zzz"))
                .arg(str)
            .arg(dir == 0 ? "收" : "发"));
    }
    else{
        if(dir == 0)
            ui->tbReadData->setPlainText(
                ui->tbReadData->toPlainText() + str
                );
    }
}

void MainWindow::closeUdp()
{
    udpSocket->close();
    udpSocket->deleteLater();
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead,
            this, &MainWindow::udpReadyReadSlot);
    ui->btnUdpBind->setText(tr("绑定"));
    ui->leUdpLocalPort->setEnabled(true);
}

void MainWindow::closeTcpClient()
{
    tcpClientSocket->disconnectFromHost();
    ui->leTcpClientServerIP->setEnabled(true);
    ui->leTcpClientServerPort->setEnabled(true);
    ui->btnTcpClientConnect->setText(tr("连接服务器"));
}

void MainWindow::closeTcpServer()
{
    foreach (QTcpSocket *s, tcpServerSocketList) {
        s->disconnectFromHost();
    }
    tcpServerSocketList.clear();
    tcpServer->close();
    ui->btnTcpServerStart->setText(tr("启动服务器"));
    ui->leTcpServerPort->setEnabled(true);
}

void MainWindow::closeSerialport()
{
    serial->close();
    ui->cbSerialBaudrate->setEnabled(true);
    ui->btnSerialMore->setEnabled(true);
    ui->btnSerialOpen->setText(tr("打开串口"));
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == autoSendTimer){
        on_btnSend_clicked();
    }
}

bool MainWindow::autoSendData(const QByteArray &sendData)
{
    MainWindow::ConnectType type = getNowType();
    bool re = false;
    switch (type) {
    case MainWindow::Serial:
        re = sendUseSerial(sendData);
        break;
    case MainWindow::TcpServer:
        re = sendUseTcpServer(sendData);
        break;
    case MainWindow::TcpClient:
        re = sendUseTcpClient(sendData);
        break;
    case MainWindow::Udp:
        re = sendUseUdp(sendData);
        break;
    default:
        break;
    }
    return re;
}

MainWindow::ConnectType MainWindow::getNowType()
{
    int index = ui->cbName->currentIndex();
    if(index < 0)
        return MainWindow::NullDev;
    if(index < ui->cbName->count() - 3)
        return MainWindow::Serial;
    else if(index == ui->cbName->count() - 3)
        return MainWindow::TcpServer;
    else if(index == ui->cbName->count() - 2)
        return MainWindow::TcpClient;
    else if(index == ui->cbName->count() - 1)
        return MainWindow::Udp;
    return MainWindow::NullDev;
}

void MainWindow::on_cbName_currentIndexChanged(int index)
{
    if(index < 0)
        return;
    if(index < ui->cbName->count() - 3)
        ui->swMain->setCurrentIndex(0);
    else if(index == ui->cbName->count() - 3)
        ui->swMain->setCurrentIndex(1);
    else if(index == ui->cbName->count() - 2)
        ui->swMain->setCurrentIndex(2);
    else if(index == ui->cbName->count() - 1)
        ui->swMain->setCurrentIndex(3);
}

void MainWindow::on_btnSend_clicked()
{
    QString temp = ui->teSendData->toPlainText();
    if(temp.isEmpty()){
        execWarnMsg(tr("发送框为空"));
        if(ui->cbSendAuto->isChecked())
            ui->cbSendAuto->setChecked(false);
        return;
    }
    QByteArray sendData;
    if(ui->cbHexSend->isChecked()){
        sendData = QByteArray::fromHex(
            ui->teSendData->toPlainText()
                .remove(QRegularExpression("\\s"))
                .toLocal8Bit());
    }
    else{
        sendData = ui->teSendData->toPlainText().toLocal8Bit();
    }
    bool re = autoSendData(sendData);
    if(re){
        showDataToUI(sendData, 1);
        if(ui->cbAutoClearSend->isChecked())
            ui->teSendData->clear();
    }
    else{
        if(ui->cbSendAuto->isChecked())
            ui->cbSendAuto->setChecked(false);
    }
}

void MainWindow::on_btnUdpBind_clicked()
{
    if(udpSocket->state() == QUdpSocket::BoundState){
        closeUdp();
    }
    else{
        quint16 port = ui->leUdpLocalPort->text().toUInt();
        bool ok = udpSocket->bind(port);
        if(ok){
            ui->btnUdpBind->setText(tr("解绑"));
            ui->leUdpLocalPort->setEnabled(false);
        }
    }
}

void MainWindow::on_cbHexSend_clicked(bool checked)
{
    if(checked){
        normalStr = ui->teSendData->toPlainText();
        ui->teSendData->setPlainText(hexStr);
    }
    else{
        hexStr = ui->teSendData->toPlainText();
        ui->teSendData->setPlainText(normalStr);
    }
}

void MainWindow::on_teSendData_textChanged()
{
    if(!ui->cbHexSend->isChecked())
        return;
    QString text = ui->teSendData->toPlainText();		//获取QTextEdit中的纯文本
    QTextCursor cur = ui->teSendData->textCursor();	//获取光标，默认获取的光标在最后
    int pos = cur.position() - 1;	//QString索引从0开始，因此减1是为了获取最后一个位置的字符的索引
    if(pos >= 0)	//pos为-1说明当前QTextEdit中没有文本
    {
        QChar ch = text.at(pos);
        if((ch >= '0' && ch <= '9')		//判断是否符合条件
            || (ch >= 'A' && ch <= 'F')
            || (ch >= 'a' && ch <= 'f')
            || (ch == ' '))
        {
            return;
        }
        else
        {
            cur.deletePreviousChar();   //删除光标所在位置的前一个字符
            execWarnMsg(tr("只能输入HEX字符"));
        }
    }
}

void MainWindow::on_btnTcpClientConnect_clicked()
{
    if(tcpClientSocket->state() == QTcpSocket::ConnectedState){
        closeTcpClient();
    }
    else{
        QString ip = ui->leTcpClientServerIP->text();
        quint16 port = ui->leTcpClientServerPort->text().toUInt();
        if(ip.isEmpty() || port == 0){
            execWarnMsg(tr("请输入服务器ip和端口"));
            return;
        }
        tcpClientSocket->connectToHost(ip, port);
        bool ok = tcpClientSocket->waitForConnected();
        if(ok){
            ui->leTcpClientServerIP->setEnabled(false);
            ui->leTcpClientServerPort->setEnabled(false);
            ui->btnTcpClientConnect->setText(tr("断开连接"));
        }
    }
}

void MainWindow::on_swMain_currentChanged(int arg1)
{
    if(arg1 == 0){
        closeTcpServer();
        closeTcpClient();
        closeUdp();
    }
    else if(arg1 == 1){
        closeSerialport();
        closeTcpClient();
        closeUdp();
    }
    else if(arg1 == 2){
        closeSerialport();
        closeTcpServer();
        closeUdp();
    }
    else if(arg1 == 3){
        closeSerialport();
        closeTcpClient();
        closeTcpServer();
    }
}

void MainWindow::on_btnTcpServerStart_clicked()
{
    if(tcpServer->isListening()){
        closeTcpServer();
    }
    else{
        quint16 port = ui->leTcpServerPort->text().toUInt();
        if(port == 0){
            execWarnMsg(tr("请输入监听端口"));
            return;
        }
        bool ok = tcpServer->listen(
            QHostAddress::Any, port);
        if(ok){
            ui->leTcpServerPort->setEnabled(false);
            ui->btnTcpServerStart->setText(tr("停止服务器"));
        }
    }
}

void MainWindow::on_btnSerialOpen_clicked()
{
    if(serial->isOpen()){
        closeSerialport();
    }
    else{
        serial->setPortName(ui->cbName->currentText());
        serial->setBaudRate(ui->cbSerialBaudrate->currentText().toUInt());
        bool ok = serial->open(QIODevice::ReadWrite);
        if(ok){
            ui->cbSerialBaudrate->setEnabled(false);
            ui->btnSerialMore->setEnabled(false);
            ui->btnSerialOpen->setText(tr("关闭串口"));
        }
    }
}

void MainWindow::on_btnSerialMore_clicked()
{
    int r = SerialMoreSet::getDialog()->exec();
    if(r == 0)
        return;
    int data,stop,parity,flow;
    SerialMoreSet::getDialog()->getConfig(data, stop, parity, flow);
    serial->setDataBits((QSerialPort::DataBits)data);
    serial->setStopBits((QSerialPort::StopBits)stop);
    serial->setParity((QSerialPort::Parity)parity);
    serial->setFlowControl((QSerialPort::FlowControl)flow);
}

void MainWindow::on_btnAddManualName_clicked()
{
    QString name = ui->leSerialManualName->text();
    if(name.isEmpty()){
        execWarnMsg(tr("端口不能为空"));
        return;
    }
    bool ok = true;
    for(int i = 0; i < ui->cbName->count(); i++) {
        if(name == ui->cbName->currentText()){
            ok = false;
            break;
        }
    }
    if(!ok){
        execWarnMsg(tr("端口不能重复"));
        return;
    }
    ui->cbName->insertItem(ui->cbName->count() - 3, name);
    ui->leSerialManualName->clear();
}

void MainWindow::on_btnClear_clicked()
{
    ui->tbReadData->clear();
}

void MainWindow::on_btnFlushNameList_clicked()
{
    closeSerialport();
    closeTcpServer();
    closeTcpClient();
    closeUdp();
    initPort();
}

void MainWindow::on_cbAutoClearSend_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
        ui->cbSendAuto->setChecked(false);
}


void MainWindow::on_cbSendAuto_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked){
        int time = ui->leAutoSendTime->text().toUInt();
        if(time == 0){
            execWarnMsg(tr("请输入时间"));
            ui->cbSendAuto->setChecked(false);
            return;
        }
        ui->cbAutoClearSend->setChecked(false);
        autoSendTimer = startTimer(time);
    }
    else{
        if(autoSendTimer >= 0){
            killTimer(autoSendTimer);
            autoSendTimer = -1;
        }
    }
}

void MainWindow::on_btnOpenFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("选择文件"));
    if(path.isEmpty())
        return;
    ui->leFilePath->setText(path);
}

void MainWindow::on_btnSendFile_clicked()
{
    QString path = ui->leFilePath->text();
    if(path.isEmpty()){
        execWarnMsg(tr("未选择文件"));
        return;
    }
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        execWarnMsg(tr("文件打开失败"));
        return;
    }
    QFileInfo info(path);
    qint64 size = info.size();
    qint64 posSize = 0;
    qint64 pos = 0;
    char buf[1024];
    bool ok;
    showDataToUI(tr("文件发送开始\n").toLocal8Bit());
    while(1){
        qint64 l = file.read(buf, 1024);
        if(l <= 0){
            showDataToUI(tr("文件发送完成\n").toLocal8Bit());
            break;
        }
        QByteArray data;
        data.append(buf, l);
        ok = autoSendData(data);
        if(!ok){
            showDataToUI(tr("文件发送失败\n").toLocal8Bit());
            break;
        }
        posSize += l;
        pos = posSize * 100 / size;
        ui->pbSendFilePos->setValue(pos);
    }
    file.close();
}

void MainWindow::on_btnSave_clicked()
{
    QString data = ui->tbReadData->toPlainText();
    if(data.isEmpty()){
        execWarnMsg(tr("接收区无数据"));
        return;
    }
    QString path = QFileDialog::getSaveFileName(
        this, tr("保存数据"), QString(), tr("文本文件 (*.txt);;所有文件 (*)"));
    if(path.isEmpty())
        return;
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        execWarnMsg(tr("文件保存失败"));
        return;
    }
    file.write(data.toUtf8());
    file.close();
}

