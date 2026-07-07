#include "transportmanager.h"
#include "transportinterface.h"

TransportManager::TransportManager(QObject *parent)
    : QObject(parent) {}

void TransportManager::registerTransport(TransportInterface *transport)
{
    if (!transport)
        return;

    transport->setParent(this);
    m_transports.append(transport);

    connect(transport, &TransportInterface::readyRead, this, [this, transport](const QByteArray &data) {
        emit payloadReady(data, transport->displayName(), static_cast<int>(TransportInterface::Direction::Receive));
    });
    connect(transport, &TransportInterface::connectionStateChanged, this, [this, transport](bool connected, const QString &message) {
        if (transport != m_transports.value(m_activeIndex))
            return;

        m_connected = connected;
        m_connectionMessage = message;
        emit connectionStateChanged();
    });
    connect(transport, &TransportInterface::errorOccurred, this, &TransportManager::errorOccurred);

    emit transportListChanged();

    if (m_activeIndex == -1)
        setActiveTransportIndex(0);
}

void TransportManager::removeTransport(int index)
{
    if (index < 0 || index >= m_transports.size())
        return;

    TransportInterface *transport = m_transports.takeAt(index);
    transport->close();
    transport->deleteLater();

    if (m_activeIndex >= m_transports.size())
        m_activeIndex = m_transports.isEmpty() ? -1 : m_transports.size() - 1;

    emit transportListChanged();
    emit activeTransportIndexChanged();
    emit connectionStateChanged();
}

QVariantList TransportManager::availableTransports() const
{
    QVariantList result;
    result.reserve(m_transports.size());
    for (TransportInterface *transport : m_transports) {
        QVariantMap map;
        map.insert("name", transport->displayName());
        map.insert("type", transport->transportType());
        map.insert("connected", transport->isOpen());
        result.append(map);
    }
    return result;
}

bool TransportManager::openCurrentTransport()
{
    TransportInterface *transport = m_transports.value(m_activeIndex);
    if (!transport)
        return false;

    const bool ok = transport->open();
    emit connectionStateChanged();
    return ok;
}

void TransportManager::closeCurrentTransport()
{
    TransportInterface *transport = m_transports.value(m_activeIndex);
    if (!transport)
        return;

    transport->close();
    emit connectionStateChanged();
}

bool TransportManager::sendData(const QByteArray &data)
{
    TransportInterface *transport = m_transports.value(m_activeIndex);
    if (!transport || !transport->isOpen())
        return false;

    const bool ok = transport->send(data);
    if (ok)
        emit payloadReady(data, transport->displayName(), static_cast<int>(TransportInterface::Direction::Send));

    return ok;
}

void TransportManager::refreshAvailablePorts()
{
    emit transportListChanged();
}

int TransportManager::activeTransportIndex() const
{
    return m_activeIndex;
}

void TransportManager::setActiveTransportIndex(int index)
{
    if (index == m_activeIndex)
        return;

    if (index < -1 || index >= m_transports.size())
        return;

    m_activeIndex = index;
    m_connected = false;
    m_connectionMessage.clear();

    if (m_activeIndex >= 0) {
        TransportInterface *transport = m_transports.value(m_activeIndex);
        if (transport) {
            m_connected = transport->isOpen();
            m_connectionMessage = m_connected ? transport->displayName() : tr("未连接");
        }
    }

    emit activeTransportIndexChanged();
    emit connectionStateChanged();
}

bool TransportManager::isConnected() const
{
    return m_connected;
}

QString TransportManager::connectionMessage() const
{
    return m_connectionMessage;
}
