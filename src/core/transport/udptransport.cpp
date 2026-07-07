#include "udptransport.h"

UdpTransport::UdpTransport(QObject *parent)
    : AbstractTransport(parent)
    , m_socket(new QUdpSocket(this))
{
    // 连接数据接收信号
    connect(m_socket, &QUdpSocket::readyRead, this, &UdpTransport::onReadyRead);
}

UdpTransport::~UdpTransport()
{
    close();
}

void UdpTransport::setConfig(const UdpConfig &config)
{
    m_config = config;
}

bool UdpTransport::open()
{
    // 绑定本地端口用于接收数据
    bool ok = m_socket->bind(m_config.localPort);
    if (ok) {
        emit connectedChanged();
    } else {
        emit errorOccurred(m_socket->errorString());
    }
    return ok;
}

void UdpTransport::close()
{
    if (m_socket->state() == QUdpSocket::BoundState) {
        m_socket->close();
        emit connectedChanged();
    }
}

bool UdpTransport::isConnected() const
{
    // 绑定状态用于接收数据
    return m_socket->state() == QUdpSocket::BoundState;
}

bool UdpTransport::sendData(const QByteArray &data)
{
    // 检查远程IP和端口是否有效
    if (m_config.peerHost.isEmpty() || m_config.peerPort == 0) {
        emit errorOccurred(tr("请输入对方ip和端口"));
        return false;
    }

    // UDP发送不需要绑定本地端口，直接发送即可
    qint64 len = m_socket->writeDatagram(data, QHostAddress(m_config.peerHost), m_config.peerPort);
    return len == data.size();
}

QString UdpTransport::errorString() const
{
    return m_socket->errorString();
}

void UdpTransport::onReadyRead()
{
    // 读取数据报
    char buf[2048];
    qint64 len = m_socket->readDatagram(buf, 2048);
    if (len > 0) {
        QByteArray data(buf, len);
        emit dataReceived(data);
    }
}
