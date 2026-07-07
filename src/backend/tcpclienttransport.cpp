#include "tcpclienttransport.h"

TcpClientTransport::TcpClientTransport(QObject *parent)
    : TransportInterface(parent)
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &TcpClientTransport::handleReadyRead);
    connect(&m_socket, &QTcpSocket::disconnected, this, &TcpClientTransport::handleDisconnected);
}

bool TcpClientTransport::open()
{
    if (m_socket.state() != QAbstractSocket::UnconnectedState)
        return true;

    m_socket.connectToHost(m_host, m_port);
    if (!m_socket.waitForConnected(3000)) {
        emit errorOccurred(m_socket.errorString());
        return false;
    }

    emit connectionStateChanged(true, tr("TCP客户端已连接"));
    return true;
}

void TcpClientTransport::close()
{
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        m_socket.disconnectFromHost();
        if (m_socket.state() != QAbstractSocket::UnconnectedState)
            m_socket.waitForDisconnected(1000);
    }
}

bool TcpClientTransport::isOpen() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

bool TcpClientTransport::send(const QByteArray &data)
{
    if (!isOpen())
        return false;

    const qint64 written = m_socket.write(data);
    if (written == -1) {
        emit errorOccurred(m_socket.errorString());
        return false;
    }

    return m_socket.flush();
}

QString TcpClientTransport::displayName() const
{
    return m_host + ":" + QString::number(m_port);
}

QString TcpClientTransport::transportType() const
{
    return QStringLiteral("tcp_client");
}

void TcpClientTransport::setTarget(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
}

void TcpClientTransport::handleReadyRead()
{
    const QByteArray data = m_socket.readAll();
    if (!data.isEmpty())
        emit readyRead(data);
}

void TcpClientTransport::handleDisconnected()
{
    emit connectionStateChanged(false, tr("TCP客户端已断开"));
    emit disconnected();
}
