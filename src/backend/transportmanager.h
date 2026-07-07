#ifndef TRANSPORTMANAGER_H
#define TRANSPORTMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class TransportInterface;

class TransportManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int activeTransportIndex READ activeTransportIndex WRITE setActiveTransportIndex NOTIFY activeTransportIndexChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)
    Q_PROPERTY(QString connectionMessage READ connectionMessage NOTIFY connectionStateChanged)

public:
    explicit TransportManager(QObject *parent = nullptr);

    Q_INVOKABLE void registerTransport(TransportInterface *transport);
    Q_INVOKABLE void removeTransport(int index);
    Q_INVOKABLE QVariantList availableTransports() const;
    Q_INVOKABLE bool openCurrentTransport();
    Q_INVOKABLE void closeCurrentTransport();
    Q_INVOKABLE bool sendData(const QByteArray &data);
    Q_INVOKABLE void refreshAvailablePorts();

    int activeTransportIndex() const;
    void setActiveTransportIndex(int index);
    bool isConnected() const;
    QString connectionMessage() const;

signals:
    void activeTransportIndexChanged();
    void connectionStateChanged();
    void transportListChanged();
    void payloadReady(const QByteArray &data, const QString &transportName, int direction);
    void errorOccurred(const QString &message);

private:
    QList<TransportInterface *> m_transports;
    int m_activeIndex = -1;
    bool m_connected = false;
    QString m_connectionMessage;
};

#endif // TRANSPORTMANAGER_H
