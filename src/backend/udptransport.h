#ifndef UDPTRANSPORT_H
#define UDPTRANSPORT_H

#include "transportinterface.h"
#include <QUdpSocket>

class UdpTransport : public TransportInterface
{
    Q_OBJECT

public:
    explicit UdpTransport(QObject *parent = nullptr);

    bool open() override;
    void close() override;
    bool isOpen() const override;
    bool send(const QByteArray &data) override;
    QString displayName() const override;
    QString transportType() const override;

    Q_INVOKABLE void setBindAddress(const QString &address, quint16 port);
    Q_INVOKABLE void setTargetAddress(const QString &host, quint16 port);

private slots:
    void handleReadyRead();

private:
    QUdpSocket m_socket;
    QString m_bindAddress;
    quint16 m_bindPort = 0;
    QHostAddress m_targetHost;
    quint16 m_targetPort = 0;
    bool m_bound = false;
};

#endif // UDPTRANSPORT_H
