#ifndef TCPCLIENTTRANSPORT_H
#define TCPCLIENTTRANSPORT_H

#include "transportinterface.h"
#include <QTcpSocket>

class TcpClientTransport : public TransportInterface
{
    Q_OBJECT

public:
    explicit TcpClientTransport(QObject *parent = nullptr);

    bool open() override;
    void close() override;
    bool isOpen() const override;
    bool send(const QByteArray &data) override;
    QString displayName() const override;
    QString transportType() const override;

    Q_INVOKABLE void setTarget(const QString &host, quint16 port);

signals:
    void disconnected();

private slots:
    void handleReadyRead();
    void handleDisconnected();

private:
    QTcpSocket m_socket;
    QString m_host;
    quint16 m_port = 0;
};

#endif // TCPCLIENTTRANSPORT_H
