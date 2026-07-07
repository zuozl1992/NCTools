#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QFont>

#include "../bridge/appcontroller.h"
#include "../core/settings/settingsmanager.h"
#include "../core/dataprocessor/dataprocessor.h"
#include "../bridge/quicksendmodel.h"
#include "../core/language/languagemanager.h"

/**
 * @brief 应用程序入口函数
 *        初始化QML引擎，注册C++类型到QML，加载主界面
 */
int main(int argc, char *argv[])
{
    // 创建GUI应用
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/control.png"));
    app.setApplicationName("MySSCOM");
    app.setApplicationVersion("2.0");

    // 设置QML样式
    QQuickStyle::setStyle("Basic");

    // 创建QML引擎
    QQmlApplicationEngine engine;

    // 初始化语言管理器
    LanguageManager::instance()->init(&app, &engine);

    // 创建应用程序控制器
    AppController appController;

    // 注册C++类型到QML
    qmlRegisterSingletonInstance("MySSCOM", 1, 0, "AppController", &appController);
    qmlRegisterSingletonInstance("MySSCOM", 1, 0, "Settings", SettingsManager::instance());
    qmlRegisterSingletonInstance("MySSCOM", 1, 0, "LanguageManager", LanguageManager::instance());
    qmlRegisterType<QuickSendModel>("MySSCOM", 1, 0, "QuickSendModel");

    // 加载主QML文件
    const QUrl url(QStringLiteral("qrc:/MySSCOM/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
