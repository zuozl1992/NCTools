#pragma once

#include "abstracttransport.h"
#include "transportconfig.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>

/**
 * @brief 串口通信类
 *        封装QSerialPort，提供串口数据收发功能
 */
class SerialTransport : public AbstractTransport
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged) ///< 可用端口列表

public:
    explicit SerialTransport(QObject *parent = nullptr);
    ~SerialTransport() override;

    /**
     * @brief 设置串口配置
     * @param config 串口配置参数
     */
    void setConfig(const SerialConfig &config);

    // ---- AbstractTransport接口实现 ----
    bool open() override;                                 ///< 打开串口
    void close() override;                                ///< 关闭串口
    bool isConnected() const override;                    ///< 是否已打开
    bool sendData(const QByteArray &data) override;       ///< 发送数据
    QString errorString() const override;                 ///< 获取错误信息

    // ---- 端口管理 ----
    QStringList availablePorts() const;                   ///< 获取可用端口列表
    Q_INVOKABLE void refreshPorts();                      ///< 刷新端口列表

    /**
     * @brief 枚举系统串口
     * @return 系统串口名称列表
     */
    static QStringList enumeratePorts();

signals:
    void availablePortsChanged();                         ///< 可用端口列表变更信号

private slots:
    void onReadyRead();                                   ///< 数据接收回调

private:
    // ==================== 成员变量 ====================
    QSerialPort *m_serial;                                ///< 串口对象
    SerialConfig m_config;                                ///< 串口配置
    QStringList m_availablePorts;                         ///< 可用端口列表
};
