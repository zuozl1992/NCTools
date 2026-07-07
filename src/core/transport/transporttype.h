#pragma once

/**
 * @brief 通信类型枚举
 *        定义支持的通信方式类型
 */
enum class TransportType {
    Serial,      ///< 串口通信
    TcpServer,   ///< TCP服务器
    TcpClient,   ///< TCP客户端
    Udp          ///< UDP通信
};
