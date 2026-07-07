#pragma once

#include <QObject>
#include <QStringList>
#include <memory>
#include "transport/abstracttransport.h"
#include "transport/transporttype.h"
#include "transport/transportconfig.h"

class SerialTransport;

/**
 * @brief 通信控制器
 *        管理当前活动的通信连接，提供统一的数据收发接口
 */
class TransportController : public QObject
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)              ///< 是否已连接
    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged) ///< 可用端口列表
    Q_PROPERTY(int clientCount READ clientCount NOTIFY clientCountChanged)           ///< 客户端数量

public:
    explicit TransportController(QObject *parent = nullptr);
    ~TransportController() override;

    /**
     * @brief 切换通信类型
     * @param type 通信类型枚举
     * @param config 通信配置
     */
    void switchTransport(TransportType type, const TransportConfig &config = TransportConfig{});

    /// 关闭当前连接
    void closeCurrent();

    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @return 是否发送成功
     */
    bool sendData(const QByteArray &data);

    /// 是否已连接
    bool isConnected() const;

    /// 获取可用端口列表
    QStringList availablePorts() const;

    /// 刷新端口列表
    Q_INVOKABLE void refreshPorts();

    /// 获取客户端数量
    int clientCount() const;

    // ---- 组件访问器 ----
    TransportType currentType() const { return m_currentType; }
    AbstractTransport *currentTransport() const { return m_currentTransport.get(); }

signals:
    void connectedChanged();                      ///< 连接状态变更
    void availablePortsChanged();                 ///< 可用端口列表变更
    void clientCountChanged();                    ///< 客户端数量变更
    void dataReceived(const QByteArray &data);    ///< 数据接收
    void errorOccurred(const QString &message);   ///< 错误发生

private:
    /// 连接通信对象信号
    void connectTransportSignals();

    // ==================== 成员变量 ====================
    std::unique_ptr<AbstractTransport> m_currentTransport;  ///< 当前通信对象
    TransportType m_currentType = TransportType::Serial;    ///< 当前通信类型
};
