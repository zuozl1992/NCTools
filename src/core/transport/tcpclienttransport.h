#pragma once

#include "abstracttransport.h"
#include "transportconfig.h"
#include <QTcpSocket>

/**
 * @brief TCP客户端通信类
 *        封装QTcpSocket，提供TCP客户端连接和数据收发功能
 */
class TcpClientTransport : public AbstractTransport
{
    Q_OBJECT

public:
    explicit TcpClientTransport(QObject *parent = nullptr);
    ~TcpClientTransport() override;

    /**
     * @brief 设置客户端配置
     * @param config 客户端配置参数
     */
    void setConfig(const TcpClientConfig &config);

    // ---- AbstractTransport接口实现 ----
    bool open() override;                                 ///< 连接到服务器
    void close() override;                                ///< 断开连接
    bool isConnected() const override;                    ///< 是否已连接
    bool sendData(const QByteArray &data) override;       ///< 发送数据
    QString errorString() const override;                 ///< 获取错误信息

private slots:
    void onReadyRead();                                   ///< 数据接收回调
    void onDisconnected();                                ///< 断开连接回调

private:
    // ==================== 成员变量 ====================
    QTcpSocket *m_socket;                                 ///< TCP套接字对象
    TcpClientConfig m_config;                             ///< 客户端配置
};
