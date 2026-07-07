#include "tcpservertransport.h"
#include <QHostAddress>

TcpServerTransport::TcpServerTransport(QObject *parent)
    : TransportInterface(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &TcpServerTransport::handleNewConnection);
}

bool TcpServerTransport::open()
{
    if (m_server.isListening())
        return true;

    if (!m_server.listen(QHostAddress::Any, m_port)) {
        emit errorOccurred(m_server.errorString());
        return false;
    }

    emit connectionStateChanged(true, tr("TCP服务器已监听"));
    return true;
}

void TcpServerTransport::close()
{
    if (m_server.isListening()) {
        for (QTcpSocket *socket : std::as_const(m_clients)) {
            socket->disconnectFromHost();
            if (socket->state() != QAbstractSocket::UnconnectedState)
                socket->waitForDisconnected(200);
        }

        m_clients.clear();
        m_server.close();
        emit clientCountChanged(0);
        emit connectionStateChanged(false, tr("TCP服务器已停止"));
    }
}

bool TcpServerTransport::isOpen() const
{
    return m_server.isListening();
}

bool TcpServerTransport::send(const QByteArray &data)
{
    return broadcast(data);
}

QString TcpServerTransport::displayName() const
{
    return tr("TCP服务器 :%1").arg(m_port);
}

QString TcpServerTransport::transportType() const
{
    return QStringLiteral("tcp_server");
}

void TcpServerTransport::setListenPort(quint16 port)
{
    m_port = port;
}

bool TcpServerTransport::broadcast(const QByteArray &data)
{
    if (!m_server.isListening())
        return false;

    bool allOk = true;
    for (QTcpSocket *socket : std::as_const(m_clients)) {
        if (socket->write(data) == -1 || !socket->flush())
            allOk = false;
    }

    return allOk;
}

void TcpServerTransport::handleNewConnection()
{
    while (QTcpSocket *socket = m_server.nextPendingConnection()) {
        m_clients.append(socket);
        bindSocketSignals(socket);
        emit clientCountChanged(m_clients.size());
        emit connectionStateChanged(true, tr("新客户端已接入: %1:%2")
                                               .arg(socket->peerAddress().toString())
                                               .arg(socket->peerPort()));
    }
}

void TcpServerTransport::handleReadyRead()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    const QByteArray data = socket->readAll();
    if (!data.isEmpty())
        emit readyRead(data);
}

void TcpServerTransport::handleDisconnected()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    socket->deleteLater();
    m_clients.removeOne(socket);
    emit clientCountChanged(m_clients.size());
    emit connectionStateChanged(false, tr("客户端已断开"));
}

void TcpServerTransport::bindSocketSignals(QTcpSocket *socket)
{
    connect(socket, &QTcpSocket::readyRead, this, &TcpServerTransport::handleReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpServerTransport::handleDisconnected);
}
