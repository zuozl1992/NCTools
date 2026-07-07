#pragma once

#include <QObject>
#include <QUrl>
#include <QTimer>
#include <QFile>
#include "transportcontroller.h"
#include "quicksendmodel.h"
#include "settings/settingsmanager.h"
#include "dataprocessor/dataprocessor.h"

/**
 * @brief 应用程序主控制器
 *        整合通信管理、数据处理、设置管理等功能，提供QML调用接口
 */
class AppController : public QObject
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(TransportController* transport READ transport CONSTANT)         ///< 通信控制器
    Q_PROPERTY(SettingsManager* settings READ settings CONSTANT)               ///< 设置管理器
    Q_PROPERTY(DataProcessor* dataProcessor READ dataProcessor CONSTANT)       ///< 数据处理器
    Q_PROPERTY(QuickSendModel* quickSendModel READ quickSendModel CONSTANT)    ///< 快速发送模型
    Q_PROPERTY(QString displayText READ displayText NOTIFY displayTextChanged) ///< 显示文本
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)        ///< 是否已连接
    Q_PROPERTY(int connectionType READ connectionType WRITE setConnectionType NOTIFY connectionTypeChanged) ///< 连接类型
    Q_PROPERTY(qint64 sentBytes READ sentBytes NOTIFY sentBytesChanged)        ///< 已发送字节数
    Q_PROPERTY(qint64 receivedBytes READ receivedBytes NOTIFY receivedBytesChanged) ///< 已接收字节数

public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController() override;

    // ---- 组件访问器 ----
    TransportController *transport() const { return m_transport; }
    SettingsManager *settings() const { return SettingsManager::instance(); }
    DataProcessor *dataProcessor() const { return m_dataProcessor; }
    QuickSendModel *quickSendModel() const { return m_quickSendModel; }

    // ---- 属性访问器 ----
    QString displayText() const { return m_displayText; }
    bool isConnected() const;
    int connectionType() const { return m_connectionType; }
    void setConnectionType(int type);
    qint64 sentBytes() const { return m_sentBytes; }
    qint64 receivedBytes() const { return m_receivedBytes; }

    // ---- 数据发送 ----
    Q_INVOKABLE void sendData(const QString &text, bool hexMode);     ///< 发送数据
    Q_INVOKABLE void sendQuickSendData(int row);                      ///< 发送快速发送条目
    Q_INVOKABLE void loadAndSendFile(const QUrl &url);                ///< 加载并发送文件

    // ---- 界面操作 ----
    Q_INVOKABLE void clearDisplay();                                  ///< 清空显示区域
    Q_INVOKABLE void saveDataToFile(const QUrl &url);                 ///< 保存数据到文件
    Q_INVOKABLE void clearStatistics();                               ///< 清空统计计数

    // ---- 端口管理 ----
    Q_INVOKABLE void refreshPorts();                                  ///< 刷新端口列表
    Q_INVOKABLE void addManualPort(const QString &portName);          ///< 手动添加端口

    // ---- 连接管理 ----
    Q_INVOKABLE bool openSerialPort(const QString &portName, int baudRate);  ///< 打开串口
    Q_INVOKABLE bool startTcpServer(int port);                               ///< 启动TCP服务器
    Q_INVOKABLE bool connectTcpClient(const QString &host, int port);        ///< 连接TCP服务器
    Q_INVOKABLE bool bindUdp(int localPort);                                 ///< 绑定UDP端口
    Q_INVOKABLE void closeConnection();                                      ///< 关闭连接

    // ---- 自动发送 ----
    Q_INVOKABLE void startAutoSend(int intervalMs);                   ///< 启动定时发送
    Q_INVOKABLE void stopAutoSend();                                  ///< 停止定时发送

signals:
    // ---- 显示相关 ----
    void displayTextChanged(const QString &text);                     ///< 显示文本变更
    void sendStatusChanged(bool ok);                                  ///< 发送状态变更
    void fileProgressChanged(int percent);                            ///< 文件发送进度变更
    void errorMessage(const QString &msg);                            ///< 错误消息

    // ---- 连接相关 ----
    void connectedChanged();                                          ///< 连接状态变更
    void connectionTypeChanged();                                     ///< 连接类型变更

    // ---- 统计相关 ----
    void sentBytesChanged();                                          ///< 发送字节数变更
    void receivedBytesChanged();                                      ///< 接收字节数变更

private slots:
    // ---- 异步回调 ----
    void onDataReceived(const QByteArray &data);                      ///< 数据接收回调
    void onConnectedChanged();                                        ///< 连接状态变更回调
    void onErrorOccurred(const QString &message);                     ///< 错误发生回调
    void onAutoSendTimer();                                           ///< 定时发送触发
    void onFileSendTimer();                                           ///< 文件发送触发

private:
    // ---- 内部方法 ----
    void appendToDisplay(const QString &text);                        ///< 追加文本到显示区
    void switchToTransport(int typeIndex);                            ///< 切换通信类型
    void sendNextFileChunk();                                         ///< 发送下一个文件块
    void clearStatistics_internal();                                  ///< 内部清空统计

    // ==================== 服务组件 ====================
    TransportController *m_transport;                                 ///< 通信控制器
    DataProcessor *m_dataProcessor;                                   ///< 数据处理器
    QuickSendModel *m_quickSendModel;                                 ///< 快速发送模型

    // ==================== 定时器 ====================
    QTimer *m_autoSendTimer;                                          ///< 自动发送定时器
    QTimer *m_fileSendTimer;                                          ///< 文件发送定时器

    // ==================== 文件发送状态 ====================
    QFile *m_sendingFile = nullptr;                                   ///< 正在发送的文件
    qint64 m_fileSize = 0;                                            ///< 文件总大小
    qint64 m_fileSentSize = 0;                                        ///< 已发送文件大小

    // ==================== 统计计数 ====================
    qint64 m_sentBytes = 0;                                           ///< 发送字节总数
    qint64 m_receivedBytes = 0;                                       ///< 接收字节总数

    // ==================== 发送缓存 ====================
    QString m_displayText;                                            ///< 显示文本内容
    QString m_autoSendText;                                           ///< 自动发送文本
    QString m_lastSendText;                                           ///< 上次发送文本
    bool m_autoSendHexMode = false;                                   ///< 自动发送Hex模式
    bool m_lastSendHexMode = false;                                   ///< 上次发送Hex模式

    // ==================== 连接状态 ====================
    int m_connectionType = 0;                                         ///< 当前连接类型
};
