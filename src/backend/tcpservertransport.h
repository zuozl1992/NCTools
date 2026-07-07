#ifndef TCPSERVERTRANSPORT_H
#define TCPSERVERTRANSPORT_H

#include "transportinterface.h"
#include <QTcpServer>
#include <QTcpSocket>

class TcpServerTransport : public TransportInterface
{
    Q_OBJECT

public:
    explicit TcpServerTransport(QObject *parent = nullptr);

    bool open() override;
    void close() override;
    bool isOpen() const override;
    bool send(const QByteArray &data) override;
    QString displayName() const override;
    QString transportType() const override;

    Q_INVOKABLE void setListenPort(quint16 port);
    Q_INVOKABLE bool broadcast(const QByteArray &data);

signals:
    void clientCountChanged(int count);

private slots:
    void handleNewConnection();
    void handleReadyRead();
    void handleDisconnected();

private:
    void bindSocketSignals(QTcpSocket *socket);

    QTcpServer m_server;
    quint16 m_port = 0;
    QList<QTcpSocket *> m_clients;
};

#endif // TCPSERVERTRANSPORT_H
