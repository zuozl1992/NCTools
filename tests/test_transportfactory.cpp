#include <QtTest>
#include <QCoreApplication>
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

QTEST_MAIN(TestTransportFactory)
#include "test_transportfactory.moc"
