#include "languagemanager.h"
#include "settings/settingsmanager.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QLocale>

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

    // 从设置中加载语言，如果没有则使用系统语言
    QString savedLanguage = SettingsManager::instance()->language();
    if (!savedLanguage.isEmpty()) {
        m_currentLanguage = savedLanguage;
    } else {
        // 尝试加载系统语言
        QString systemLocale = QLocale::system().name();
        if (systemLocale.startsWith("zh")) {
            m_currentLanguage = "zh_CN";
        } else {
            m_currentLanguage = "en_US";
        }
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

    // 保存语言设置
    SettingsManager::instance()->setLanguage(language);

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

    // 加载新的翻译文件
    QString translationFile = QString(":/i18n/NCTools_%1.qm").arg(language);
    if (m_translator->load(translationFile)) {
        m_app->installTranslator(m_translator);
    }
}
