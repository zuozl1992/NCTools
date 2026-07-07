#include "udptransport.h"

UdpTransport::UdpTransport(QObject *parent)
    : TransportInterface(parent)
{
    connect(&m_socket, &QUdpSocket::readyRead, this, &UdpTransport::handleReadyRead);
}

bool UdpTransport::open()
{
    if (m_bound)
        return true;

    const QHostAddress address = m_bindAddress.isEmpty() ? QHostAddress::Any : QHostAddress(m_bindAddress);
    if (!m_socket.bind(address, m_bindPort)) {
        emit errorOccurred(m_socket.errorString());
        return false;
    }

    m_bound = true;
    emit connectionStateChanged(true, tr("UDP已绑定"));
    return true;
}

void UdpTransport::close()
{
    if (m_bound) {
        m_socket.close();
        m_bound = false;
        emit connectionStateChanged(false, tr("UDP已关闭"));
    }
}

bool UdpTransport::isOpen() const
{
    return m_bound;
}

bool UdpTransport::send(const QByteArray &data)
{
    const qint64 written = m_socket.writeDatagram(data, m_targetHost, m_targetPort);
    if (written == -1) {
        emit errorOccurred(m_socket.errorString());
        return false;
    }

    return true;
}

QString UdpTransport::displayName() const
{
    return m_targetHost.toString() + ":" + QString::number(m_targetPort);
}

QString UdpTransport::transportType() const
{
    return QStringLiteral("udp");
}

void UdpTransport::setBindAddress(const QString &address, quint16 port)
{
    m_bindAddress = address;
    m_bindPort = port;
}

void UdpTransport::setTargetAddress(const QString &host, quint16 port)
{
    m_targetHost = QHostAddress(host);
    m_targetPort = port;
}

void UdpTransport::handleReadyRead()
{
    while (m_socket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(m_socket.pendingDatagramSize()));
        qint64 size = m_socket.readDatagram(datagram.data(), datagram.size());
        if (size > 0)
            emit readyRead(datagram.left(int(size)));
    }
}
