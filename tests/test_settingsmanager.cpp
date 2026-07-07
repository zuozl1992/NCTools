#include <QtTest>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QSettings>
#include "settings/settingsmanager.h"

class TestSettingsManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDefaultValues();
    void testSerialSettings();
    void testTcpServerSettings();
    void testTcpClientSettings();
    void testUdpSettings();
    void testDisplaySettings();
    void testConnectionType();
    void testQuickSendEntries();
    void testSaveLoad();

private:
    QTemporaryDir *m_tempDir;
};

void TestSettingsManager::initTestCase()
{
    m_tempDir = new QTemporaryDir;
    QVERIFY(m_tempDir->isValid());
}

void TestSettingsManager::cleanupTestCase()
{
    delete m_tempDir;
}

void TestSettingsManager::testDefaultValues()
{
    SettingsManager *settings = SettingsManager::instance();

    // Serial defaults
    QCOMPARE(settings->serialBaudRateIndex(), 2);
    QCOMPARE(settings->serialDataBits(), 8);
    QCOMPARE(settings->serialStopBits(), 1);
    QCOMPARE(settings->serialParity(), 0);
    QCOMPARE(settings->serialFlowControl(), 0);
    QVERIFY(settings->manualPorts().isEmpty());

    // TCP Server defaults
    QCOMPARE(settings->tcpServerPort(), QString("10000"));

    // TCP Client defaults
    QCOMPARE(settings->tcpClientIP(), QString("127.0.0.1"));
    QCOMPARE(settings->tcpClientPort(), QString("10000"));

    // UDP defaults
    QCOMPARE(settings->udpLocalPort(), QString("10001"));
    QCOMPARE(settings->udpPeerIP(), QString("127.0.0.1"));
    QCOMPARE(settings->udpPeerPort(), QString("10002"));

    // Display defaults
    QCOMPARE(settings->hexShow(), false);
    QCOMPARE(settings->hexSend(), false);
    QCOMPARE(settings->autoClearSend(), false);
    QCOMPARE(settings->addTimestamp(), false);
    QCOMPARE(settings->autoSend(), false);
    QCOMPARE(settings->autoSendInterval(), 1000);

    // Connection default
    QCOMPARE(settings->connectionType(), 0);
}

void TestSettingsManager::testSerialSettings()
{
    SettingsManager *settings = SettingsManager::instance();

    // Test setters with signal verification
    QSignalSpy spyBaud(settings, &SettingsManager::serialBaudRateIndexChanged);
    settings->setSerialBaudRateIndex(1);
    QCOMPARE(settings->serialBaudRateIndex(), 1);
    QCOMPARE(spyBaud.count(), 1);

    // Setting same value should not emit signal
    settings->setSerialBaudRateIndex(1);
    QCOMPARE(spyBaud.count(), 1);

    // Test other serial settings
    QSignalSpy spyData(settings, &SettingsManager::serialDataBitsChanged);
    settings->setSerialDataBits(7);
    QCOMPARE(settings->serialDataBits(), 7);
    QCOMPARE(spyData.count(), 1);

    QSignalSpy spyStop(settings, &SettingsManager::serialStopBitsChanged);
    settings->setSerialStopBits(2);
    QCOMPARE(settings->serialStopBits(), 2);
    QCOMPARE(spyStop.count(), 1);

    QSignalSpy spyParity(settings, &SettingsManager::serialParityChanged);
    settings->setSerialParity(2);
    QCOMPARE(settings->serialParity(), 2);
    QCOMPARE(spyParity.count(), 1);

    QSignalSpy spyFlow(settings, &SettingsManager::serialFlowControlChanged);
    settings->setSerialFlowControl(1);
    QCOMPARE(settings->serialFlowControl(), 1);
    QCOMPARE(spyFlow.count(), 1);

    QSignalSpy spyPorts(settings, &SettingsManager::manualPortsChanged);
    QStringList ports = {"COM10", "/dev/ttyUSB0"};
    settings->setManualPorts(ports);
    QCOMPARE(settings->manualPorts(), ports);
    QCOMPARE(spyPorts.count(), 1);
}

void TestSettingsManager::testTcpServerSettings()
{
    SettingsManager *settings = SettingsManager::instance();

    QSignalSpy spy(settings, &SettingsManager::tcpServerPortChanged);
    settings->setTcpServerPort("8080");
    QCOMPARE(settings->tcpServerPort(), QString("8080"));
    QCOMPARE(spy.count(), 1);
}

void TestSettingsManager::testTcpClientSettings()
{
    SettingsManager *settings = SettingsManager::instance();

    QSignalSpy spyIP(settings, &SettingsManager::tcpClientIPChanged);
    settings->setTcpClientIP("192.168.1.100");
    QCOMPARE(settings->tcpClientIP(), QString("192.168.1.100"));
    QCOMPARE(spyIP.count(), 1);

    QSignalSpy spyPort(settings, &SettingsManager::tcpClientPortChanged);
    settings->setTcpClientPort("9090");
    QCOMPARE(settings->tcpClientPort(), QString("9090"));
    QCOMPARE(spyPort.count(), 1);
}

void TestSettingsManager::testUdpSettings()
{
    SettingsManager *settings = SettingsManager::instance();

    QSignalSpy spyLocal(settings, &SettingsManager::udpLocalPortChanged);
    settings->setUdpLocalPort("5000");
    QCOMPARE(settings->udpLocalPort(), QString("5000"));
    QCOMPARE(spyLocal.count(), 1);

    QSignalSpy spyIP(settings, &SettingsManager::udpPeerIPChanged);
    settings->setUdpPeerIP("10.0.0.1");
    QCOMPARE(settings->udpPeerIP(), QString("10.0.0.1"));
    QCOMPARE(spyIP.count(), 1);

    QSignalSpy spyPort(settings, &SettingsManager::udpPeerPortChanged);
    settings->setUdpPeerPort("5001");
    QCOMPARE(settings->udpPeerPort(), QString("5001"));
    QCOMPARE(spyPort.count(), 1);
}

void TestSettingsManager::testDisplaySettings()
{
    SettingsManager *settings = SettingsManager::instance();

    QSignalSpy spyHexShow(settings, &SettingsManager::hexShowChanged);
    settings->setHexShow(true);
    QCOMPARE(settings->hexShow(), true);
    QCOMPARE(spyHexShow.count(), 1);

    QSignalSpy spyHexSend(settings, &SettingsManager::hexSendChanged);
    settings->setHexSend(true);
    QCOMPARE(settings->hexSend(), true);
    QCOMPARE(spyHexSend.count(), 1);

    QSignalSpy spyAutoClear(settings, &SettingsManager::autoClearSendChanged);
    settings->setAutoClearSend(true);
    QCOMPARE(settings->autoClearSend(), true);
    QCOMPARE(spyAutoClear.count(), 1);

    QSignalSpy spyTimestamp(settings, &SettingsManager::addTimestampChanged);
    settings->setAddTimestamp(true);
    QCOMPARE(settings->addTimestamp(), true);
    QCOMPARE(spyTimestamp.count(), 1);

    QSignalSpy spyAutoSend(settings, &SettingsManager::autoSendChanged);
    settings->setAutoSend(true);
    QCOMPARE(settings->autoSend(), true);
    QCOMPARE(spyAutoSend.count(), 1);

    QSignalSpy spyInterval(settings, &SettingsManager::autoSendIntervalChanged);
    settings->setAutoSendInterval(2000);
    QCOMPARE(settings->autoSendInterval(), 2000);
    QCOMPARE(spyInterval.count(), 1);
}

void TestSettingsManager::testConnectionType()
{
    SettingsManager *settings = SettingsManager::instance();

    QSignalSpy spy(settings, &SettingsManager::connectionTypeChanged);
    settings->setConnectionType(2);
    QCOMPARE(settings->connectionType(), 2);
    QCOMPARE(spy.count(), 1);
}

void TestSettingsManager::testQuickSendEntries()
{
    SettingsManager *settings = SettingsManager::instance();

    QVariantList entries;
    QVariantMap entry1;
    entry1["data"] = "Hello";
    entry1["hex"] = false;
    entries.append(entry1);

    QVariantMap entry2;
    entry2["data"] = "414243";
    entry2["hex"] = true;
    entries.append(entry2);

    QSignalSpy spy(settings, &SettingsManager::quickSendEntriesChanged);
    settings->setQuickSendEntries(entries);
    QCOMPARE(settings->quickSendEntries().size(), 2);
    QCOMPARE(spy.count(), 1);

    QVariantMap result1 = settings->quickSendEntries()[0].toMap();
    QCOMPARE(result1["data"].toString(), QString("Hello"));
    QCOMPARE(result1["hex"].toBool(), false);

    QVariantMap result2 = settings->quickSendEntries()[1].toMap();
    QCOMPARE(result2["data"].toString(), QString("414243"));
    QCOMPARE(result2["hex"].toBool(), true);
}

void TestSettingsManager::testSaveLoad()
{
    // This test verifies the save/load mechanism works
    // In a real test, we would need to mock QStandardPaths
    // For now, we just verify the API exists and can be called
    SettingsManager *settings = SettingsManager::instance();

    // Save should not crash
    settings->save();

    // Load should not crash
    settings->load();
}

QTEST_MAIN(TestSettingsManager)
#include "test_settingsmanager.moc"
