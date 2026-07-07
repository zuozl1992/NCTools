#include "tcpclienttransport.h"

TcpClientTransport::TcpClientTransport(QObject *parent)
    : AbstractTransport(parent)
    , m_socket(new QTcpSocket(this))
{
    // 连接数据接收和断开信号
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClientTransport::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClientTransport::onDisconnected);
}

TcpClientTransport::~TcpClientTransport()
{
    close();
}

void TcpClientTransport::setConfig(const TcpClientConfig &config)
{
    m_config = config;
}

bool TcpClientTransport::open()
{
    // 连接到服务器
    m_socket->connectToHost(m_config.host, m_config.port);

    // 等待连接完成（超时3秒）
    bool ok = m_socket->waitForConnected(3000);
    if (ok) {
        emit connectedChanged();
    } else {
        emit errorOccurred(m_socket->errorString());
    }
    return ok;
}

void TcpClientTransport::close()
{
    if (m_socket->state() != QTcpSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool TcpClientTransport::isConnected() const
{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

bool TcpClientTransport::sendData(const QByteArray &data)
{
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        emit errorOccurred(tr("未连接服务器"));
        return false;
    }

    qint64 len = m_socket->write(data);
    return len == data.size();
}

QString TcpClientTransport::errorString() const
{
    return m_socket->errorString();
}

void TcpClientTransport::onReadyRead()
{
    // 读取所有可用数据
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClientTransport::onDisconnected()
{
    // 连接断开
    emit connectedChanged();
}
