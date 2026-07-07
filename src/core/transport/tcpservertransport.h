#pragma once

#include "abstracttransport.h"
#include "transportconfig.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

/**
 * @brief TCP服务器通信类
 *        封装QTcpServer，支持多客户端连接和数据广播
 */
class TcpServerTransport : public AbstractTransport
{
    Q_OBJECT

public:
    explicit TcpServerTransport(QObject *parent = nullptr);
    ~TcpServerTransport() override;

    /**
     * @brief 设置服务器配置
     * @param config 服务器配置参数
     */
    void setConfig(const TcpServerConfig &config);

    // ---- AbstractTransport接口实现 ----
    bool open() override;                                 ///< 启动服务器监听
    void close() override;                                ///< 停止服务器
    bool isConnected() const override;                    ///< 是否正在监听
    bool sendData(const QByteArray &data) override;       ///< 广播数据到所有客户端
    QString errorString() const override;                 ///< 获取错误信息

    /**
     * @brief 获取已连接客户端数量
     * @return 客户端数量
     */
    int clientCount() const;

signals:
    void clientCountChanged();                            ///< 客户端数量变更信号

private slots:
    void onNewConnection();                               ///< 新客户端连接回调
    void onClientReadyRead();                             ///< 客户端数据接收回调
    void onClientDisconnected();                          ///< 客户端断开回调

private:
    // ==================== 成员变量 ====================
    QTcpServer *m_server;                                 ///< TCP服务器对象
    QList<QTcpSocket *> m_clients;                        ///< 已连接客户端列表
    TcpServerConfig m_config;                             ///< 服务器配置
};
