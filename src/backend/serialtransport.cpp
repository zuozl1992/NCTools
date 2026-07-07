#include "serialtransport.h"
#include <QVariantMap>

SerialTransport::SerialTransport(QObject *parent)
    : TransportInterface(parent)
{
    connect(&m_port, &QSerialPort::readyRead, this, &SerialTransport::handleReadyRead);
    connect(&m_port, &QSerialPort::aboutToClose, this, &SerialTransport::handleDisconnected);
}

bool SerialTransport::open()
{
    if (m_port.isOpen())
        return true;

    if (m_portName.isEmpty())
        return false;

    m_port.setPortName(m_portName);
    m_port.setBaudRate(m_baudRate);

    const auto dataBits = static_cast<QSerialPort::DataBits>(m_settings.value("dataBits", QSerialPort::Data8).toInt());
    const auto stopBits = static_cast<QSerialPort::StopBits>(m_settings.value("stopBits", QSerialPort::OneStop).toInt());
    const auto parity = static_cast<QSerialPort::Parity>(m_settings.value("parity", QSerialPort::NoParity).toInt());
    const auto flowControl = static_cast<QSerialPort::FlowControl>(m_settings.value("flowControl", QSerialPort::NoFlowControl).toInt());

    m_port.setDataBits(dataBits);
    m_port.setStopBits(stopBits);
    m_port.setParity(parity);
    m_port.setFlowControl(flowControl);

    if (!m_port.open(QIODevice::ReadWrite)) {
        emit errorOccurred(m_port.errorString());
        return false;
    }

    emit connectionStateChanged(true, tr("串口已打开"));
    return true;
}

void SerialTransport::close()
{
    if (m_port.isOpen()) {
        m_port.close();
        emit connectionStateChanged(false, tr("串口已关闭"));
    }
}

bool SerialTransport::isOpen() const
{
    return m_port.isOpen();
}

bool SerialTransport::send(const QByteArray &data)
{
    if (!m_port.isOpen())
        return false;

    const qint64 written = m_port.write(data);
    if (written == -1) {
        emit errorOccurred(m_port.errorString());
        return false;
    }

    return m_port.flush();
}

QString SerialTransport::displayName() const
{
    return m_portName;
}

QString SerialTransport::transportType() const
{
    return QStringLiteral("serial");
}

QStringList SerialTransport::availablePorts() const
{
    QStringList ports;
    const auto infos = QSerialPortInfo::availablePorts();
    ports.reserve(infos.size());
    for (const QSerialPortInfo &info : infos)
        ports.append(info.portName());
    return ports;
}

void SerialTransport::applySettings(const QVariantMap &settings)
{
    m_settings = settings;
    emit settingsApplied(m_settings);
}

void SerialTransport::setPortName(const QString &portName)
{
    m_portName = portName;
}

void SerialTransport::setBaudRate(qint32 baudRate)
{
    m_baudRate = baudRate;
}

void SerialTransport::handleReadyRead()
{
    const QByteArray data = m_port.readAll();
    if (!data.isEmpty())
        emit readyRead(data);
}

void SerialTransport::handleDisconnected()
{
    emit connectionStateChanged(false, tr("串口已断开"));
}
