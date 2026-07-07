#pragma once

#include "abstracttransport.h"
#include "transportconfig.h"
#include <QUdpSocket>

/**
 * @brief UDP通信类
 *        封装QUdpSocket，提供UDP数据报收发功能
 *        绑定本地端口用于接收，发送只需远程IP和端口
 */
class UdpTransport : public AbstractTransport
{
    Q_OBJECT

public:
    explicit UdpTransport(QObject *parent = nullptr);
    ~UdpTransport() override;

    /**
     * @brief 设置UDP配置
     * @param config UDP配置参数
     */
    void setConfig(const UdpConfig &config);

    // ---- AbstractTransport接口实现 ----
    bool open() override;                                 ///< 绑定本地端口（用于接收）
    void close() override;                                ///< 解绑端口
    bool isConnected() const override;                    ///< 是否已绑定
    bool sendData(const QByteArray &data) override;       ///< 发送数据报
    QString errorString() const override;                 ///< 获取错误信息

private slots:
    void onReadyRead();                                   ///< 数据报接收回调

private:
    // ==================== 成员变量 ====================
    QUdpSocket *m_socket;                                 ///< UDP套接字对象
    UdpConfig m_config;                                   ///< UDP配置
};
