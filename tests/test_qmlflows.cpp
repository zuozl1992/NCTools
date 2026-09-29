#include <QtTest>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QStandardPaths>
#include "appcontroller.h"
#include "language/languagemanager.h"

class TestQmlFlows : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testSerialDialogRestoresSavedChoices();
    void testFailedSendRetainsText();
    void testAutoSendCheckboxTracksTimer();

private:
    QObject *createWindow(QQmlEngine &engine, const QString &childQml);
    AppController *m_controller = nullptr;
    QQmlEngine *m_engine = nullptr;
};

void TestQmlFlows::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    QQuickStyle::setStyle("Basic");
    m_controller = new AppController(this);
    m_engine = new QQmlEngine(this);
    qmlRegisterSingletonInstance("NCTools", 1, 0, "AppController", m_controller);
    qmlRegisterSingletonInstance("NCTools", 1, 0, "LanguageManager", LanguageManager::instance());
}

QObject *TestQmlFlows::createWindow(QQmlEngine &engine, const QString &childQml)
{
    const QString sourceDir = QStringLiteral(NCTOOLS_APP_SOURCE_DIR);
    const QString source = QStringLiteral("import QtQuick\nimport QtQuick.Controls\n"
        "import NCTools 1.0\nimport \"%1\" as Views\n"
        "ApplicationWindow { visible: true; width: 640; height: 480; %2 }")
        .arg(QUrl::fromLocalFile(sourceDir).toString(), childQml);
    QQmlComponent component(&engine);
    component.setData(source.toUtf8(), QUrl::fromLocalFile(sourceDir + "/TestHarness.qml"));
    QObject *window = component.create();
    if (!window)
        qWarning().noquote() << component.errorString();
    return window;
}

void TestQmlFlows::testSerialDialogRestoresSavedChoices()
{
    auto *settings = SettingsManager::instance();
    settings->setSerialDataBits(7);
    settings->setSerialStopBits(2);
    settings->setSerialParity(3);
    settings->setSerialFlowControl(2);

    QScopedPointer<QObject> window(createWindow(*m_engine,
        "Views.SerialSettingsDialog { objectName: \"dialog\" }"));
    QVERIFY(window);
    QObject *dialog = window->findChild<QObject *>("dialog");
    QVERIFY(dialog);
    QVERIFY(QMetaObject::invokeMethod(dialog, "open"));
    QTRY_VERIFY(dialog->property("visible").toBool());
    QCOMPARE(dialog->findChild<QObject *>("serialDataBits")->property("currentIndex").toInt(), 2);
    QCOMPARE(dialog->findChild<QObject *>("serialStopBits")->property("currentIndex").toInt(), 2);
    QCOMPARE(dialog->findChild<QObject *>("serialParity")->property("currentIndex").toInt(), 2);
    QCOMPARE(dialog->findChild<QObject *>("serialFlowControl")->property("currentIndex").toInt(), 2);
    dialog->findChild<QObject *>("serialDataBits")->setProperty("currentIndex", 0);
    QVERIFY(QMetaObject::invokeMethod(dialog, "close"));
    QVERIFY(QMetaObject::invokeMethod(dialog, "open"));
    QTRY_COMPARE(dialog->findChild<QObject *>("serialDataBits")->property("currentIndex").toInt(), 2);
    QCOMPARE(settings->serialDataBits(), 7);
}

void TestQmlFlows::testFailedSendRetainsText()
{
    m_controller->closeConnection();
    SettingsManager::instance()->setAutoClearSend(true);
    QScopedPointer<QObject> window(createWindow(*m_engine,
        "Views.SendPanel { objectName: \"panel\"; anchors.fill: parent }"));
    QVERIFY(window);
    QObject *panel = window->findChild<QObject *>("panel");
    QVERIFY(panel);
    QObject *textArea = panel->findChild<QObject *>("sendTextArea");
    QObject *button = panel->findChild<QObject *>("sendButton");
    QVERIFY(textArea);
    QVERIFY(button);
    textArea->setProperty("text", "retain me");
    QSignalSpy sendStatus(m_controller, &AppController::sendStatusChanged);
    QVERIFY(QMetaObject::invokeMethod(button, "clicked"));
    QCOMPARE(sendStatus.count(), 1);
    QCOMPARE(sendStatus.at(0).at(0).toBool(), false);
    QCOMPARE(textArea->property("text").toString(), QString("retain me"));
}

void TestQmlFlows::testAutoSendCheckboxTracksTimer()
{
    m_controller->closeConnection();
    QVERIFY(!m_controller->sendData("payload", false));
    QQmlComponent component(m_engine,
        QUrl::fromLocalFile(QStringLiteral(NCTOOLS_APP_SOURCE_DIR) + "/Main.qml"));
    QScopedPointer<QObject> window(component.create());
    if (!window)
        qWarning().noquote() << component.errorString();
    QVERIFY(window);
    QObject *checkBox = window->property("autoSendCheckBoxObject").value<QObject *>();
    QObject *interval = window->property("autoSendIntervalObject").value<QObject *>();
    QVERIFY(checkBox);
    QVERIFY(interval);
    interval->setProperty("text", "10");
    checkBox->setProperty("checked", true);
    QVERIFY(QMetaObject::invokeMethod(checkBox, "clicked"));
    QVERIFY(m_controller->autoSendRunning());
    QTRY_VERIFY(!m_controller->autoSendRunning());
    QTRY_VERIFY(!checkBox->property("checked").toBool());
}

QTEST_MAIN(TestQmlFlows)
#include "test_qmlflows.moc"
