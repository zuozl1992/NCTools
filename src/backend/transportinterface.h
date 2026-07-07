#ifndef TRANSPORTINTERFACE_H
#define TRANSPORTINTERFACE_H

#include <QObject>
#include <QByteArray>

class TransportInterface : public QObject
{
    Q_OBJECT

public:
    enum class Direction { Receive = 0, Send = 1, System = 2 };
    Q_ENUM(Direction)

    explicit TransportInterface(QObject *parent = nullptr)
        : QObject(parent) {}

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual bool send(const QByteArray &data) = 0;
    virtual QString displayName() const = 0;
    virtual QString transportType() const = 0;

signals:
    void readyRead(const QByteArray &data);
    void connectionStateChanged(bool connected, const QString &message);
    void errorOccurred(const QString &message);
};

#endif // TRANSPORTINTERFACE_H
