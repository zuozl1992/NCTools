#include <QtTest>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTemporaryFile>
#ifdef Q_OS_UNIX
#include <unistd.h>
#endif
#include "appcontroller.h"

class TestAppController : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testFileSendDrainsBeforeCompletion();
    void testFileSendFailsWhenConnectionCloses();
    void testFileReadErrorIsFailure();
    void testInvalidPortsAreRejected();
    void testAutoSendNeedsPayload();
    void testAutoSendStopsAfterSendFailure();
    void testQuickSendUpdatesStatistics();
    void testDisplayTextIsBounded();
    void testSavedConnectionModeIsRestored();
    void testSerialPortsRefreshWhenReturningToMode();
};

void TestAppController::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void TestAppController::testFileSendDrainsBeforeCompletion()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    AppController controller;
    QVERIFY(controller.connectTcpClient("127.0.0.1", server.serverPort()));
    QTRY_VERIFY(server.hasPendingConnections());
    QScopedPointer<QTcpSocket> peer(server.nextPendingConnection());

    QTemporaryFile file;
    QVERIFY(file.open());
    const QByteArray expected(32 * 1024, 'x');
    QCOMPARE(file.write(expected), expected.size());
    QVERIFY(file.flush());

    QSignalSpy progress(&controller, &AppController::fileProgressChanged);
    controller.loadAndSendFile(QUrl::fromLocalFile(file.fileName()));

    QByteArray received;
    QElapsedTimer elapsed;
    elapsed.start();
    while (elapsed.elapsed() < 5000 && received.size() < expected.size()) {
        QCoreApplication::processEvents();
        if (peer->waitForReadyRead(20))
            received += peer->readAll();
    }
    received += peer->readAll();
    QCOMPARE(received, expected);
    QTRY_VERIFY(progress.count() > 0 && progress.last().at(0).toInt() == 100);
    QVERIFY(controller.displayText().contains("[文件发送完成]"));
}

void TestAppController::testFileSendFailsWhenConnectionCloses()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    AppController controller;
    QVERIFY(controller.connectTcpClient("127.0.0.1", server.serverPort()));

    QTemporaryFile file;
    QVERIFY(file.open());
    QCOMPARE(file.write(QByteArray(32 * 1024, 'y')), qint64(32 * 1024));
    QVERIFY(file.flush());

    QSignalSpy progress(&controller, &AppController::fileProgressChanged);
    controller.loadAndSendFile(QUrl::fromLocalFile(file.fileName()));
    controller.closeConnection();
    QTRY_VERIFY(progress.count() > 0 && progress.last().at(0).toInt() == -1);
    QVERIFY(controller.displayText().contains("[文件发送失败]"));
}

void TestAppController::testFileReadErrorIsFailure()
{
#ifndef Q_OS_UNIX
    QSKIP("该故障注入使用 POSIX 文件描述符");
#else
    QTemporaryFile file;
    QVERIFY(file.open());
    QCOMPARE(file.write("read error", 10), qint64(10));
    QVERIFY(file.flush());
    AppController controller;
    QSignalSpy progress(&controller, &AppController::fileProgressChanged);
    controller.loadAndSendFile(QUrl::fromLocalFile(file.fileName()));
    QFile *sendingFile = controller.findChild<QFile *>();
    QVERIFY(sendingFile);
    QVERIFY(sendingFile->handle() >= 0);
    QCOMPARE(::close(sendingFile->handle()), 0);
    QTRY_VERIFY(progress.count() > 0 && progress.last().at(0).toInt() == -1);
    QVERIFY(controller.displayText().contains("[文件发送失败]"));
#endif
}

void TestAppController::testInvalidPortsAreRejected()
{
    AppController controller;
    QSignalSpy errors(&controller, &AppController::errorMessage);
    QVERIFY(!controller.startTcpServer(0));
    QVERIFY(!controller.startTcpServer(65536));
    QVERIFY(!controller.connectTcpClient("", 1));
    QVERIFY(!controller.bindUdp(65536));
    QVERIFY(errors.count() >= 4);
    QVERIFY(!controller.isConnected());
}

void TestAppController::testAutoSendNeedsPayload()
{
    AppController controller;
    QSignalSpy errors(&controller, &AppController::errorMessage);
    controller.startAutoSend(10);
    QVERIFY(!controller.autoSendRunning());
    QVERIFY(errors.count() >= 1);
}

void TestAppController::testAutoSendStopsAfterSendFailure()
{
    AppController controller;
    QVERIFY(!controller.sendData("payload", false));
    controller.startAutoSend(10);
    QVERIFY(controller.autoSendRunning());
    QTRY_VERIFY(!controller.autoSendRunning());
    controller.startAutoSend(1000);
    QVERIFY(controller.autoSendRunning());
    controller.closeConnection();
    QVERIFY(!controller.autoSendRunning());
}

void TestAppController::testQuickSendUpdatesStatistics()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    AppController controller;
    QVERIFY(controller.connectTcpClient("127.0.0.1", server.serverPort()));
    QTRY_VERIFY(server.hasPendingConnections());
    QScopedPointer<QTcpSocket> peer(server.nextPendingConnection());
    controller.quickSendModel()->setEntryData(0, "ABC");
    controller.quickSendModel()->setEntryHex(0, false);
    QCOMPARE(controller.sentBytes(), qint64(0));
    controller.sendQuickSendData(0);
    QTRY_VERIFY(peer->bytesAvailable() >= 3);
    QCOMPARE(peer->readAll(), QByteArray("ABC"));
    QCOMPARE(controller.sentBytes(), qint64(3));
}

void TestAppController::testDisplayTextIsBounded()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    AppController controller;
    QVERIFY(controller.connectTcpClient("127.0.0.1", server.serverPort()));
    QTRY_VERIFY(server.hasPendingConnections());
    QScopedPointer<QTcpSocket> peer(server.nextPendingConnection());
    const QByteArray payload(2 * 1024 * 1024, 'z');
    QVERIFY(peer->write(payload) > 0);
    QVERIFY(peer->waitForBytesWritten(3000));
    QTRY_VERIFY(controller.receivedBytes() == payload.size());
    QVERIFY(controller.displayText().size() <= 1024 * 1024);
    QVERIFY(controller.displayText().endsWith(QString(4096, QChar('z'))));
}

void TestAppController::testSavedConnectionModeIsRestored()
{
    SettingsManager::instance()->setConnectionType(2);
    AppController controller;
    QCOMPARE(controller.connectionType(), 2);
    QVERIFY(!controller.isConnected());
    SettingsManager::instance()->setConnectionType(0);
}

void TestAppController::testSerialPortsRefreshWhenReturningToMode()
{
    AppController controller;
    QSignalSpy refreshed(controller.transport(), &TransportController::availablePortsChanged);
    controller.setConnectionType(1);
    controller.setConnectionType(0);
    QVERIFY(refreshed.count() >= 1);
}

QTEST_GUILESS_MAIN(TestAppController)
#include "test_appcontroller.moc"
