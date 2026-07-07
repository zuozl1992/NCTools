#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H

#include "transportinterface.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVariantMap>

class SerialTransport : public TransportInterface
{
    Q_OBJECT

public:
    explicit SerialTransport(QObject *parent = nullptr);

    bool open() override;
    void close() override;
    bool isOpen() const override;
    bool send(const QByteArray &data) override;
    QString displayName() const override;
    QString transportType() const override;

    Q_INVOKABLE QStringList availablePorts() const;
    Q_INVOKABLE void applySettings(const QVariantMap &settings);
    Q_INVOKABLE void setPortName(const QString &portName);
    Q_INVOKABLE void setBaudRate(qint32 baudRate);

signals:
    void settingsApplied(const QVariantMap &settings);

private slots:
    void handleReadyRead();
    void handleDisconnected();

private:
    QSerialPort m_port;
    QVariantMap m_settings;
    QString m_portName;
    qint32 m_baudRate = 9600;
};

#endif // SERIALTRANSPORT_H
