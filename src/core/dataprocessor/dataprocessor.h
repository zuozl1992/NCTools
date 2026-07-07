#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief 数据处理器
 *        提供数据格式转换功能，包括Hex编解码、时间戳添加等
 */
class DataProcessor : public QObject
{
    Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);

    // ---- Hex转换 ----
    /**
     * @brief 将字节数组转换为Hex字符串
     * @param data 字节数组
     * @return Hex字符串（如"48 65 6C 6C 6F"）
     */
    Q_INVOKABLE static QString toHexString(const QByteArray &data);

    /**
     * @brief 将Hex字符串转换为字节数组
     * @param hex Hex字符串
     * @return 字节数组
     */
    Q_INVOKABLE static QByteArray fromHexString(const QString &hex);

    /**
     * @brief 将字节数组转换为本地编码字符串
     * @param data 字节数组
     * @return 本地编码字符串
     */
    Q_INVOKABLE static QString toLocalString(const QByteArray &data);

    // ---- 时间戳 ----
    /**
     * @brief 为文本添加时间戳
     * @param text 原始文本
     * @param isSend 是否为发送数据
     * @return 带时间戳的文本
     */
    Q_INVOKABLE static QString appendTimestamp(const QString &text, bool isSend);

    // ---- Hex验证 ----
    /**
     * @brief 验证输入是否为有效的Hex字符
     * @param text 输入文本
     * @param cursorPos 光标位置
     * @return 是否有效
     */
    Q_INVOKABLE static bool isValidHexInput(const QString &text, int cursorPos);

    /**
     * @brief 过滤文本中的非Hex字符
     * @param text 原始文本
     * @return 过滤后的文本
     */
    Q_INVOKABLE static QString filterHexString(const QString &text);
};
