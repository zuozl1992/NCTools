#pragma once

#include <QString>
#include <QSerialPort>
#include <variant>

/**
 * @brief 串口配置结构体
 */
struct SerialConfig {
    QString portName;                                      ///< 端口名称
    qint32 baudRate = 115200;                              ///< 波特率
    QSerialPort::DataBits dataBits = QSerialPort::Data8;   ///< 数据位
    QSerialPort::StopBits stopBits = QSerialPort::OneStop; ///< 停止位
    QSerialPort::Parity parity = QSerialPort::NoParity;    ///< 校验位
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl; ///< 流控制
};

/**
 * @brief TCP服务器配置结构体
 */
struct TcpServerConfig {
    quint16 listenPort = 10000;    ///< 监听端口
};

/**
 * @brief TCP客户端配置结构体
 */
struct TcpClientConfig {
    QString host = "127.0.0.1";   ///< 服务器地址
    quint16 port = 10000;         ///< 服务器端口
};

/**
 * @brief UDP配置结构体
 */
struct UdpConfig {
    quint16 localPort = 10001;            ///< 本地端口（用于接收）
    QString peerHost = "127.0.0.1";       ///< 远程主机地址
    quint16 peerPort = 10002;             ///< 远程主机端口
};

/**
 * @brief 通信配置联合类型
 *        使用std::variant存储不同通信类型的配置
 */
using TransportConfig = std::variant<SerialConfig, TcpServerConfig, TcpClientConfig, UdpConfig>;
