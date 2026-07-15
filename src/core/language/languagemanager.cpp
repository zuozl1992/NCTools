#include "languagemanager.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QLocale>
#include <QDebug>
#include <QFile>

static LanguageManager *s_instance = nullptr;

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
    , m_translator(new QTranslator(this))
{
}

LanguageManager *LanguageManager::instance()
{
    if (!s_instance) {
        s_instance = new LanguageManager;
    }
    return s_instance;
}

void LanguageManager::init(QGuiApplication *app, QQmlEngine *engine)
{
    m_app = app;
    m_engine = engine;

    // 尝试加载系统语言
    QString systemLocale = QLocale::system().name();
    if (systemLocale.startsWith("zh")) {
        m_currentLanguage = "zh_CN";
    } else {
        m_currentLanguage = "en_US";
    }

    // 加载翻译
    loadTranslation(m_currentLanguage);
}

void LanguageManager::switchLanguage(const QString &language)
{
    if (m_currentLanguage == language)
        return;

    m_currentLanguage = language;
    loadTranslation(language);

    emit currentLanguageChanged();

    // 重新加载QML以应用翻译
    if (m_engine) {
        m_engine->retranslate();
    }
}

void LanguageManager::loadTranslation(const QString &language)
{
    if (!m_app)
        return;

    // 移除旧的翻译器
    m_app->removeTranslator(m_translator);

    // 加载新的翻译文件（资源路径）
    QString translationFile = QString(":/i18n/NCTools_%1.qm").arg(language);
    qDebug() << "Loading translation from:" << translationFile;
    qDebug() << "Translation exists:" << QFile::exists(translationFile);

    if (m_translator->load(translationFile)) {
        m_app->installTranslator(m_translator);
        qDebug() << "Translation loaded successfully";
        // 测试翻译是否生效
        qDebug() << "Test translate '串口':" << m_translator->translate("StatusBar", "串口");
    } else {
        qDebug() << "Failed to load translation";
    }
}
