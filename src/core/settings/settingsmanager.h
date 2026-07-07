#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class QSettings;

/**
 * @brief 设置管理器
 *        管理应用程序的所有配置项，支持持久化存储
 *        使用单例模式，提供全局访问点
 */
class SettingsManager : public QObject
{
    Q_OBJECT

    // ==================== 串口设置 ====================
    Q_PROPERTY(int serialBaudRateIndex READ serialBaudRateIndex WRITE setSerialBaudRateIndex NOTIFY serialBaudRateIndexChanged) ///< 波特率索引
    Q_PROPERTY(int serialDataBits READ serialDataBits WRITE setSerialDataBits NOTIFY serialDataBitsChanged) ///< 数据位
    Q_PROPERTY(int serialStopBits READ serialStopBits WRITE setSerialStopBits NOTIFY serialStopBitsChanged) ///< 停止位
    Q_PROPERTY(int serialParity READ serialParity WRITE setSerialParity NOTIFY serialParityChanged) ///< 校验位
    Q_PROPERTY(int serialFlowControl READ serialFlowControl WRITE setSerialFlowControl NOTIFY serialFlowControlChanged) ///< 流控制
    Q_PROPERTY(QStringList manualPorts READ manualPorts WRITE setManualPorts NOTIFY manualPortsChanged) ///< 手动添加的端口列表

    // ==================== TCP服务器设置 ====================
    Q_PROPERTY(QString tcpServerPort READ tcpServerPort WRITE setTcpServerPort NOTIFY tcpServerPortChanged) ///< 监听端口

    // ==================== TCP客户端设置 ====================
    Q_PROPERTY(QString tcpClientIP READ tcpClientIP WRITE setTcpClientIP NOTIFY tcpClientIPChanged) ///< 服务器IP
    Q_PROPERTY(QString tcpClientPort READ tcpClientPort WRITE setTcpClientPort NOTIFY tcpClientPortChanged) ///< 服务器端口

    // ==================== UDP设置 ====================
    Q_PROPERTY(QString udpLocalPort READ udpLocalPort WRITE setUdpLocalPort NOTIFY udpLocalPortChanged) ///< 本地端口
    Q_PROPERTY(QString udpPeerIP READ udpPeerIP WRITE setUdpPeerIP NOTIFY udpPeerIPChanged) ///< 远程IP
    Q_PROPERTY(QString udpPeerPort READ udpPeerPort WRITE setUdpPeerPort NOTIFY udpPeerPortChanged) ///< 远程端口

    // ==================== 显示设置 ====================
    Q_PROPERTY(bool hexShow READ hexShow WRITE setHexShow NOTIFY hexShowChanged) ///< Hex显示模式
    Q_PROPERTY(bool hexSend READ hexSend WRITE setHexSend NOTIFY hexSendChanged) ///< Hex发送模式
    Q_PROPERTY(bool autoClearSend READ autoClearSend WRITE setAutoClearSend NOTIFY autoClearSendChanged) ///< 发送后自动清空
    Q_PROPERTY(bool addTimestamp READ addTimestamp WRITE setAddTimestamp NOTIFY addTimestampChanged) ///< 添加时间戳
    Q_PROPERTY(bool autoSend READ autoSend WRITE setAutoSend NOTIFY autoSendChanged) ///< 自动发送
    Q_PROPERTY(int autoSendInterval READ autoSendInterval WRITE setAutoSendInterval NOTIFY autoSendIntervalChanged) ///< 自动发送间隔

    // ==================== 连接设置 ====================
    Q_PROPERTY(int connectionType READ connectionType WRITE setConnectionType NOTIFY connectionTypeChanged) ///< 连接类型

    // ==================== 快速发送设置 ====================
    Q_PROPERTY(QVariantList quickSendEntries READ quickSendEntries WRITE setQuickSendEntries NOTIFY quickSendEntriesChanged) ///< 快速发送条目

public:
    /**
     * @brief 获取单例实例
     * @return SettingsManager实例指针
     */
    static SettingsManager *instance();

    // ---- 持久化操作 ----
    Q_INVOKABLE void save();                              ///< 保存设置到文件
    Q_INVOKABLE void load();                              ///< 从文件加载设置

    // ---- 串口属性访问器 ----
    int serialBaudRateIndex() const { return m_serialBaudRateIndex; }
    int serialDataBits() const { return m_serialDataBits; }
    int serialStopBits() const { return m_serialStopBits; }
    int serialParity() const { return m_serialParity; }
    int serialFlowControl() const { return m_serialFlowControl; }
    QStringList manualPorts() const { return m_manualPorts; }

    // ---- 串口属性设置器 ----
    void setSerialBaudRateIndex(int index);
    void setSerialDataBits(int bits);
    void setSerialStopBits(int bits);
    void setSerialParity(int parity);
    void setSerialFlowControl(int flow);
    void setManualPorts(const QStringList &ports);

    // ---- TCP服务器属性 ----
    QString tcpServerPort() const { return m_tcpServerPort; }
    void setTcpServerPort(const QString &port);

    // ---- TCP客户端属性 ----
    QString tcpClientIP() const { return m_tcpClientIP; }
    QString tcpClientPort() const { return m_tcpClientPort; }
    void setTcpClientIP(const QString &ip);
    void setTcpClientPort(const QString &port);

    // ---- UDP属性 ----
    QString udpLocalPort() const { return m_udpLocalPort; }
    QString udpPeerIP() const { return m_udpPeerIP; }
    QString udpPeerPort() const { return m_udpPeerPort; }
    void setUdpLocalPort(const QString &port);
    void setUdpPeerIP(const QString &ip);
    void setUdpPeerPort(const QString &port);

    // ---- 显示属性 ----
    bool hexShow() const { return m_hexShow; }
    bool hexSend() const { return m_hexSend; }
    bool autoClearSend() const { return m_autoClearSend; }
    bool addTimestamp() const { return m_addTimestamp; }
    bool autoSend() const { return m_autoSend; }
    int autoSendInterval() const { return m_autoSendInterval; }
    void setHexShow(bool show);
    void setHexSend(bool send);
    void setAutoClearSend(bool clear);
    void setAddTimestamp(bool add);
    void setAutoSend(bool autoSend);
    void setAutoSendInterval(int interval);

    // ---- 连接属性 ----
    int connectionType() const { return m_connectionType; }
    void setConnectionType(int type);

    // ---- 快速发送属性 ----
    QVariantList quickSendEntries() const { return m_quickSendEntries; }
    void setQuickSendEntries(const QVariantList &entries);

signals:
    // ---- 串口信号 ----
    void serialBaudRateIndexChanged();                    ///< 波特率变更
    void serialDataBitsChanged();                         ///< 数据位变更
    void serialStopBitsChanged();                         ///< 停止位变更
    void serialParityChanged();                           ///< 校验位变更
    void serialFlowControlChanged();                      ///< 流控制变更
    void manualPortsChanged();                            ///< 手动端口列表变更

    // ---- TCP服务器信号 ----
    void tcpServerPortChanged();                          ///< 服务器端口变更

    // ---- TCP客户端信号 ----
    void tcpClientIPChanged();                            ///< 客户端IP变更
    void tcpClientPortChanged();                          ///< 客户端端口变更

    // ---- UDP信号 ----
    void udpLocalPortChanged();                           ///< 本地端口变更
    void udpPeerIPChanged();                              ///< 远程IP变更
    void udpPeerPortChanged();                            ///< 远程端口变更

    // ---- 显示信号 ----
    void hexShowChanged();                                ///< Hex显示变更
    void hexSendChanged();                                ///< Hex发送变更
    void autoClearSendChanged();                          ///< 自动清空变更
    void addTimestampChanged();                           ///< 时间戳变更
    void autoSendChanged();                               ///< 自动发送变更
    void autoSendIntervalChanged();                       ///< 自动发送间隔变更

    // ---- 连接信号 ----
    void connectionTypeChanged();                         ///< 连接类型变更

    // ---- 快速发送信号 ----
    void quickSendEntriesChanged();                       ///< 快速发送条目变更

private:
    explicit SettingsManager(QObject *parent = nullptr);

    // ==================== 串口配置 ====================
    int m_serialBaudRateIndex = 2;                        ///< 波特率索引（默认115200）
    int m_serialDataBits = 8;                             ///< 数据位（默认Data8）
    int m_serialStopBits = 1;                             ///< 停止位（默认OneStop）
    int m_serialParity = 0;                               ///< 校验位（默认NoParity）
    int m_serialFlowControl = 0;                          ///< 流控制（默认NoFlowControl）
    QStringList m_manualPorts;                            ///< 手动添加的端口列表

    // ==================== TCP服务器配置 ====================
    QString m_tcpServerPort = "10000";                    ///< 监听端口

    // ==================== TCP客户端配置 ====================
    QString m_tcpClientIP = "127.0.0.1";                  ///< 服务器IP
    QString m_tcpClientPort = "10000";                    ///< 服务器端口

    // ==================== UDP配置 ====================
    QString m_udpLocalPort = "10001";                     ///< 本地端口
    QString m_udpPeerIP = "127.0.0.1";                    ///< 远程IP
    QString m_udpPeerPort = "10002";                      ///< 远程端口

    // ==================== 显示配置 ====================
    bool m_hexShow = false;                               ///< Hex显示模式
    bool m_hexSend = false;                               ///< Hex发送模式
    bool m_autoClearSend = false;                         ///< 发送后自动清空
    bool m_addTimestamp = false;                          ///< 添加时间戳
    bool m_autoSend = false;                              ///< 自动发送
    int m_autoSendInterval = 1000;                        ///< 自动发送间隔（毫秒）

    // ==================== 连接配置 ====================
    int m_connectionType = 0;                             ///< 连接类型

    // ==================== 快速发送配置 ====================
    QVariantList m_quickSendEntries;                      ///< 快速发送条目列表
};
