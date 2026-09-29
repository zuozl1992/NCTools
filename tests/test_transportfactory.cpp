#include <QtTest>
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include "transportcontroller.h"
#include "transport/transportfactory.h"
#include "transport/transporttype.h"
#include "transport/transportconfig.h"
#include "transport/serialtransport.h"
#include "transport/tcpservertransport.h"
#include "transport/tcpclienttransport.h"
#include "transport/udptransport.h"

class TestTransportFactory : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testCreateSerial();
    void testCreateTcpServer();
    void testCreateTcpClient();
    void testCreateUdp();
    void testCreateWithSerialConfig();
    void testCreateWithTcpServerConfig();
    void testCreateWithTcpClientConfig();
    void testCreateWithUdpConfig();
    void testTcpServerUsesConfiguredPort();
    void testUdpReceivesFullDatagram();
    void testTcpServerReleasesDisconnectedClients();
    void testUdpDrainsQueuedDatagrams();
    void testTransportControllerClientCount();
};

void TestTransportFactory::initTestCase()
{
}

void TestTransportFactory::cleanupTestCase()
{
}

void TestTransportFactory::testCreateSerial()
{
    auto transport = TransportFactory::create(TransportType::Serial, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<SerialTransport *>(transport.get()) != nullptr);
    QCOMPARE(transport->isConnected(), false);
}

void TestTransportFactory::testCreateTcpServer()
{
    auto transport = TransportFactory::create(TransportType::TcpServer, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<TcpServerTransport *>(transport.get()) != nullptr);
    QCOMPARE(transport->isConnected(), false);
}

void TestTransportFactory::testCreateTcpClient()
{
    auto transport = TransportFactory::create(TransportType::TcpClient, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<TcpClientTransport *>(transport.get()) != nullptr);
    QCOMPARE(transport->isConnected(), false);
}

void TestTransportFactory::testCreateUdp()
{
    auto transport = TransportFactory::create(TransportType::Udp, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<UdpTransport *>(transport.get()) != nullptr);
    QCOMPARE(transport->isConnected(), false);
}

void TestTransportFactory::testCreateWithSerialConfig()
{
    SerialConfig config;
    config.portName = "COM1";
    config.baudRate = 9600;
    TransportConfig variant = config;
    auto transport = TransportFactory::create(variant, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<SerialTransport *>(transport.get()) != nullptr);
}

void TestTransportFactory::testCreateWithTcpServerConfig()
{
    TcpServerConfig config;
    config.listenPort = 8080;
    TransportConfig variant = config;
    auto transport = TransportFactory::create(variant, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<TcpServerTransport *>(transport.get()) != nullptr);
}

void TestTransportFactory::testCreateWithTcpClientConfig()
{
    TcpClientConfig config;
    config.host = "192.168.1.1";
    config.port = 9090;
    TransportConfig variant = config;
    auto transport = TransportFactory::create(variant, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<TcpClientTransport *>(transport.get()) != nullptr);
}

void TestTransportFactory::testCreateWithUdpConfig()
{
    UdpConfig config;
    config.localPort = 5000;
    config.peerHost = "10.0.0.1";
    config.peerPort = 5001;
    TransportConfig variant = config;
    auto transport = TransportFactory::create(variant, this);
    QVERIFY(transport != nullptr);
    QVERIFY(dynamic_cast<UdpTransport *>(transport.get()) != nullptr);
}

void TestTransportFactory::testTcpServerUsesConfiguredPort()
{
    QTcpServer portProbe;
    QVERIFY(portProbe.listen(QHostAddress::LocalHost, 0));
    const quint16 port = portProbe.serverPort();
    portProbe.close();

    TransportController controller;
    TcpServerConfig config;
    config.listenPort = port;
    controller.switchTransport(TransportType::TcpServer, config);
    QVERIFY(controller.currentTransport()->open());

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, port);
    QVERIFY2(client.waitForConnected(3000), qPrintable(client.errorString()));
    client.disconnectFromHost();
    controller.closeCurrent();
}

void TestTransportFactory::testUdpReceivesFullDatagram()
{
    QUdpSocket portProbe;
    QVERIFY(portProbe.bind(QHostAddress::LocalHost, 0));
    const quint16 port = portProbe.localPort();
    portProbe.close();

    UdpTransport transport;
    UdpConfig config;
    config.localPort = port;
    transport.setConfig(config);
    QVERIFY(transport.open());

    QSignalSpy received(&transport, &AbstractTransport::dataReceived);
    QUdpSocket sender;
    const QByteArray payload(4096, 'u');
    QCOMPARE(sender.writeDatagram(payload, QHostAddress::LocalHost, port), payload.size());
    QTRY_COMPARE(received.count(), 1);
    QCOMPARE(received.at(0).at(0).toByteArray(), payload);
}

void TestTransportFactory::testTcpServerReleasesDisconnectedClients()
{
    TcpServerTransport server;
    TcpServerConfig config;
    config.listenPort = 0;
    server.setConfig(config);
    QVERIFY(server.open());
    QTcpServer *listener = server.findChild<QTcpServer *>();
    QVERIFY(listener);

    for (int i = 0; i < 10; ++i) {
        QTcpSocket client;
        client.connectToHost(QHostAddress::LocalHost, listener->serverPort());
        QVERIFY(client.waitForConnected(3000));
        QTRY_COMPARE(server.clientCount(), 1);
        client.disconnectFromHost();
        QTRY_COMPARE(server.clientCount(), 0);
        QTRY_COMPARE(server.findChildren<QTcpSocket *>().size(), 0);
    }
}

void TestTransportFactory::testUdpDrainsQueuedDatagrams()
{
    QUdpSocket portProbe;
    QVERIFY(portProbe.bind(QHostAddress::LocalHost, 0));
    const quint16 port = portProbe.localPort();
    portProbe.close();

    UdpTransport transport;
    UdpConfig config;
    config.localPort = port;
    transport.setConfig(config);
    QVERIFY(transport.open());
    QSignalSpy received(&transport, &AbstractTransport::dataReceived);

    QUdpSocket sender;
    for (int i = 0; i < 10; ++i) {
        const QByteArray payload = QByteArray::number(i);
        QCOMPARE(sender.writeDatagram(payload, QHostAddress::LocalHost, port), payload.size());
    }
    QTRY_COMPARE(received.count(), 10);
    for (int i = 0; i < 10; ++i)
        QCOMPARE(received.at(i).at(0).toByteArray(), QByteArray::number(i));
}

void TestTransportFactory::testTransportControllerClientCount()
{
    QTcpServer portProbe;
    QVERIFY(portProbe.listen(QHostAddress::LocalHost, 0));
    const quint16 port = portProbe.serverPort();
    portProbe.close();

    TransportController controller;
    QSignalSpy counts(&controller, &TransportController::clientCountChanged);
    TcpServerConfig config;
    config.listenPort = port;
    controller.switchTransport(TransportType::TcpServer, config);
    QVERIFY(controller.currentTransport()->open());
    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, port);
    QVERIFY(client.waitForConnected(3000));
    QTRY_COMPARE(controller.clientCount(), 1);
    QVERIFY(counts.count() >= 1);
    client.disconnectFromHost();
    QTRY_COMPARE(controller.clientCount(), 0);
    QVERIFY(counts.count() >= 2);
}

QTEST_MAIN(TestTransportFactory)
#include "test_transportfactory.moc"
