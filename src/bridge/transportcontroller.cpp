#include "transportcontroller.h"
#include "transport/transportfactory.h"
#include "transport/serialtransport.h"
#include "transport/tcpservertransport.h"
#include "transport/tcpclienttransport.h"
#include "transport/udptransport.h"

TransportController::TransportController(QObject *parent)
    : QObject(parent)
{
}

TransportController::~TransportController()
{
    closeCurrent();
}

void TransportController::switchTransport(TransportType type, const TransportConfig &config)
{
    // 关闭当前连接
    closeCurrent();

    // 创建新的通信对象
    m_currentType = type;
    m_currentTransport = TransportFactory::create(type, this);
    if (m_currentTransport) {
        // 连接信号
        connectTransportSignals();

        // 应用配置
        std::visit([this](const auto &cfg) {
            using T = std::decay_t<decltype(cfg)>;
            if constexpr (std::is_same_v<T, SerialConfig>) {
                if (auto *serial = dynamic_cast<SerialTransport *>(m_currentTransport.get())) {
                    serial->setConfig(cfg);
                }
            } else if constexpr (std::is_same_v<T, TcpServerConfig>) {
                if (auto *server = dynamic_cast<TcpServerTransport *>(m_currentTransport.get())) {
                    server->setConfig(cfg);
                }
            } else if constexpr (std::is_same_v<T, TcpClientConfig>) {
                if (auto *client = dynamic_cast<TcpClientTransport *>(m_currentTransport.get())) {
                    client->setConfig(cfg);
                }
            } else if constexpr (std::is_same_v<T, UdpConfig>) {
                if (auto *udp = dynamic_cast<UdpTransport *>(m_currentTransport.get())) {
                    udp->setConfig(cfg);
                }
            }
        }, config);
    }
}

void TransportController::closeCurrent()
{
    if (m_currentTransport) {
        m_currentTransport->close();
        m_currentTransport.reset();
        emit connectedChanged();
        emit clientCountChanged();
    }
}

bool TransportController::sendData(const QByteArray &data)
{
    if (!m_currentTransport) {
        emit errorOccurred(tr("没有活动的连接"));
        return false;
    }
    return m_currentTransport->sendData(data);
}

bool TransportController::isConnected() const
{
    return m_currentTransport && m_currentTransport->isConnected();
}

QStringList TransportController::availablePorts() const
{
    // 仅串口通信支持端口列表
    if (auto *serial = dynamic_cast<SerialTransport *>(m_currentTransport.get())) {
        return serial->availablePorts();
    }
    return {};
}

void TransportController::refreshPorts()
{
    // 仅串口通信支持刷新端口
    if (auto *serial = dynamic_cast<SerialTransport *>(m_currentTransport.get())) {
        serial->refreshPorts();
    }
}

int TransportController::clientCount() const
{
    if (auto *server = dynamic_cast<TcpServerTransport *>(m_currentTransport.get()))
        return server->clientCount();
    return 0;
}

void TransportController::connectTransportSignals()
{
    if (!m_currentTransport)
        return;

    // 连接通用信号
    connect(m_currentTransport.get(), &AbstractTransport::dataReceived,
            this, &TransportController::dataReceived);
    connect(m_currentTransport.get(), &AbstractTransport::connectedChanged,
            this, &TransportController::connectedChanged);
    connect(m_currentTransport.get(), &AbstractTransport::errorOccurred,
            this, &TransportController::errorOccurred);

    // 连接串口特有信号
    if (auto *serial = dynamic_cast<SerialTransport *>(m_currentTransport.get())) {
        connect(serial, &SerialTransport::availablePortsChanged,
                this, &TransportController::availablePortsChanged);
    }

    if (auto *server = dynamic_cast<TcpServerTransport *>(m_currentTransport.get())) {
        connect(server, &TcpServerTransport::clientCountChanged,
                this, &TransportController::clientCountChanged);
    }
}
