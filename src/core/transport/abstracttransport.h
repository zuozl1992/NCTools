#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief 通信传输抽象基类
 *        定义所有通信类型的统一接口，支持串口、TCP、UDP等通信方式
 */
class AbstractTransport : public QObject
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)      ///< 是否已连接
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorOccurred)    ///< 错误信息

public:
    explicit AbstractTransport(QObject *parent = nullptr);
    ~AbstractTransport() override;

    /**
     * @brief 打开连接
     * @return 是否成功打开
     */
    virtual bool open() = 0;

    /// 关闭连接
    virtual void close() = 0;

    /**
     * @brief 是否已连接
     * @return 连接状态
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @return 是否发送成功
     */
    virtual bool sendData(const QByteArray &data) = 0;

    /**
     * @brief 获取错误信息
     * @return 错误描述字符串
     */
    virtual QString errorString() const = 0;

signals:
    void dataReceived(const QByteArray &data);    ///< 数据接收信号
    void connectedChanged();                      ///< 连接状态变更信号
    void errorOccurred(const QString &message);   ///< 错误发生信号
};
