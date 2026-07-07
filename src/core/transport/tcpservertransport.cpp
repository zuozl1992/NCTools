#include "tcpservertransport.h"

TcpServerTransport::TcpServerTransport(QObject *parent)
    : AbstractTransport(parent)
    , m_server(new QTcpServer(this))
{
    // 连接新连接信号
    connect(m_server, &QTcpServer::newConnection, this, &TcpServerTransport::onNewConnection);
}

TcpServerTransport::~TcpServerTransport()
{
    close();
}

void TcpServerTransport::setConfig(const TcpServerConfig &config)
{
    m_config = config;
}

bool TcpServerTransport::open()
{
    // 开始监听指定端口
    bool ok = m_server->listen(QHostAddress::Any, m_config.listenPort);
    if (ok) {
        emit connectedChanged();
    } else {
        emit errorOccurred(m_server->errorString());
    }
    return ok;
}

void TcpServerTransport::close()
{
    // 断开所有客户端连接
    foreach (QTcpSocket *client, m_clients) {
        client->disconnectFromHost();
    }
    m_clients.clear();

    // 停止服务器监听
    m_server->close();
    emit connectedChanged();
    emit clientCountChanged();
}

bool TcpServerTransport::isConnected() const
{
    return m_server->isListening();
}

bool TcpServerTransport::sendData(const QByteArray &data)
{
    if (m_clients.isEmpty()) {
        emit errorOccurred(tr("没有已连接的客户端"));
        return false;
    }

    // 广播数据到所有已连接的客户端
    bool allSuccess = true;
    foreach (QTcpSocket *client, m_clients) {
        qint64 len = client->write(data);
        if (len != data.size()) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

QString TcpServerTransport::errorString() const
{
    return m_server->errorString();
}

int TcpServerTransport::clientCount() const
{
    return m_clients.size();
}

void TcpServerTransport::onNewConnection()
{
    // 处理所有待处理的连接
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        m_clients.append(socket);

        // 连接客户端信号
        connect(socket, &QTcpSocket::readyRead, this, &TcpServerTransport::onClientReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &TcpServerTransport::onClientDisconnected);

        // 保存客户端信息（IP和端口）
        QHostAddress ip = socket->peerAddress();
        quint16 port = socket->peerPort();
        bool ok = false;
        quint32 ipU32 = ip.toIPv4Address(&ok);
        QString ipStr;
        if (ok) {
            // 格式化IPv4地址
            quint8 ia[4];
            ia[0] = ipU32;
            ia[1] = ipU32 >> 8;
            ia[2] = ipU32 >> 16;
            ia[3] = ipU32 >> 24;
            ipStr = QString("%1.%2.%3.%4")
                        .arg(ia[3]).arg(ia[2]).arg(ia[1]).arg(ia[0]);
        } else {
            ipStr = ip.toString();
        }

        // 将客户端信息存储为属性
        socket->setProperty("ip_str", ipStr);
        socket->setProperty("port_str", QString("%1").arg(port));

        emit clientCountChanged();
    }
}

void TcpServerTransport::onClientReadyRead()
{
    // 接收客户端数据
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        QByteArray data = socket->readAll();
        emit dataReceived(data);
    }
}

void TcpServerTransport::onClientDisconnected()
{
    // 客户端断开连接
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        disconnect(socket, nullptr, nullptr, nullptr);
        m_clients.removeOne(socket);
        emit clientCountChanged();
    }
}
