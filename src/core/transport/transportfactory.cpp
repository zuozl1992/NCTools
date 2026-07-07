#include "transportfactory.h"
#include "serialtransport.h"
#include "tcpservertransport.h"
#include "tcpclienttransport.h"
#include "udptransport.h"

std::unique_ptr<AbstractTransport> TransportFactory::create(TransportType type, QObject *parent)
{
    // 根据通信类型创建对应的通信对象
    switch (type) {
    case TransportType::Serial:
        return std::make_unique<SerialTransport>(parent);
    case TransportType::TcpServer:
        return std::make_unique<TcpServerTransport>(parent);
    case TransportType::TcpClient:
        return std::make_unique<TcpClientTransport>(parent);
    case TransportType::Udp:
        return std::make_unique<UdpTransport>(parent);
    default:
        return nullptr;
    }
}

std::unique_ptr<AbstractTransport> TransportFactory::create(const TransportConfig &config, QObject *parent)
{
    // 根据配置类型创建对应的通信对象并应用配置
    return std::visit([parent](const auto &cfg) -> std::unique_ptr<AbstractTransport> {
        using T = std::decay_t<decltype(cfg)>;

        if constexpr (std::is_same_v<T, SerialConfig>) {
            auto transport = std::make_unique<SerialTransport>(parent);
            transport->setConfig(cfg);
            return transport;
        } else if constexpr (std::is_same_v<T, TcpServerConfig>) {
            auto transport = std::make_unique<TcpServerTransport>(parent);
            transport->setConfig(cfg);
            return transport;
        } else if constexpr (std::is_same_v<T, TcpClientConfig>) {
            auto transport = std::make_unique<TcpClientTransport>(parent);
            transport->setConfig(cfg);
            return transport;
        } else if constexpr (std::is_same_v<T, UdpConfig>) {
            auto transport = std::make_unique<UdpTransport>(parent);
            transport->setConfig(cfg);
            return transport;
        }

        return nullptr;
    }, config);
}
