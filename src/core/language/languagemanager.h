#pragma once

#include <QObject>
#include <QTranslator>
#include <QStringList>

class QGuiApplication;
class QQmlEngine;

/**
 * @brief 语言管理器
 *        管理应用程序的国际化语言切换
 */
class LanguageManager : public QObject
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged) ///< 当前语言
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)           ///< 可用语言列表

public:
    static LanguageManager *instance();

    /**
     * @brief 初始化语言管理器
     * @param app 应用程序实例
     * @param engine QML引擎实例
     */
    void init(QGuiApplication *app, QQmlEngine *engine);

    /**
     * @brief 切换语言
     * @param language 语言代码（"zh_CN" 或 "en_US"）
     */
    Q_INVOKABLE void switchLanguage(const QString &language);

    /**
     * @brief 获取当前语言
     * @return 语言代码
     */
    QString currentLanguage() const { return m_currentLanguage; }

    /**
     * @brief 获取可用语言列表
     * @return 语言代码列表
     */
    QStringList availableLanguages() const { return {"zh_CN", "en_US"}; }

signals:
    void currentLanguageChanged();                        ///< 当前语言变更信号

private:
    explicit LanguageManager(QObject *parent = nullptr);

    /**
     * @brief 加载翻译文件
     * @param language 语言代码
     */
    void loadTranslation(const QString &language);

    // ==================== 成员变量 ====================
    QGuiApplication *m_app = nullptr;                     ///< 应用程序实例
    QQmlEngine *m_engine = nullptr;                       ///< QML引擎实例
    QTranslator *m_translator = nullptr;                  ///< 翻译器
    QString m_currentLanguage = "zh_CN";                  ///< 当前语言
};
