#include "settingsmanager.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

static SettingsManager *s_instance = nullptr;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

SettingsManager *SettingsManager::instance()
{
    if (!s_instance) {
        s_instance = new SettingsManager;
    }
    return s_instance;
}

void SettingsManager::save()
{
    // 配置文件路径：~/Library/Application Support/MySSCOM/config.ini
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                         + "/MySSCOM";
    QDir().mkpath(configPath);
    QSettings settings(configPath + "/config.ini", QSettings::IniFormat);

    // 保存串口配置
    settings.beginGroup("Serial");
    settings.setValue("baudrate", m_serialBaudRateIndex);
    settings.setValue("databits", m_serialDataBits);
    settings.setValue("stopbits", m_serialStopBits);
    settings.setValue("parity", m_serialParity);
    settings.setValue("flowcontrol", m_serialFlowControl);
    settings.setValue("manualPorts", m_manualPorts);
    settings.endGroup();

    // 保存TCP服务器配置
    settings.beginGroup("TCPServer");
    settings.setValue("port", m_tcpServerPort);
    settings.endGroup();

    // 保存TCP客户端配置
    settings.beginGroup("TCPClient");
    settings.setValue("ip", m_tcpClientIP);
    settings.setValue("port", m_tcpClientPort);
    settings.endGroup();

    // 保存UDP配置
    settings.beginGroup("UDP");
    settings.setValue("localPort", m_udpLocalPort);
    settings.setValue("peerIP", m_udpPeerIP);
    settings.setValue("peerPort", m_udpPeerPort);
    settings.endGroup();

    // 保存显示配置
    settings.beginGroup("Display");
    settings.setValue("hexShow", m_hexShow);
    settings.setValue("hexSend", m_hexSend);
    settings.setValue("autoClearSend", m_autoClearSend);
    settings.setValue("addTime", m_addTimestamp);
    settings.setValue("sendAuto", m_autoSend);
    settings.setValue("autoSendTime", m_autoSendInterval);
    settings.endGroup();

    // 保存连接配置
    settings.beginGroup("General");
    settings.setValue("connectionType", m_connectionType);
    settings.endGroup();

    // 保存快速发送条目
    settings.beginGroup("ExpPanel");
    settings.setValue("rowCount", m_quickSendEntries.size());
    for (int i = 0; i < m_quickSendEntries.size(); i++) {
        QVariantMap entry = m_quickSendEntries[i].toMap();
        settings.setValue(QString("data_%1").arg(i), entry["data"].toString());
        settings.setValue(QString("hex_%1").arg(i), entry["hex"].toBool());
    }
    settings.endGroup();
}

void SettingsManager::load()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                         + "/MySSCOM/config.ini";
    if (!QFile::exists(configPath))
        return;

    QSettings settings(configPath, QSettings::IniFormat);

    // 加载串口配置
    settings.beginGroup("Serial");
    m_serialBaudRateIndex = settings.value("baudrate", 2).toInt();
    m_serialDataBits = settings.value("databits", 8).toInt();
    m_serialStopBits = settings.value("stopbits", 1).toInt();
    m_serialParity = settings.value("parity", 0).toInt();
    m_serialFlowControl = settings.value("flowcontrol", 0).toInt();
    m_manualPorts = settings.value("manualPorts").toStringList();
    settings.endGroup();

    // 加载TCP服务器配置
    settings.beginGroup("TCPServer");
    m_tcpServerPort = settings.value("port", "10000").toString();
    settings.endGroup();

    // 加载TCP客户端配置
    settings.beginGroup("TCPClient");
    m_tcpClientIP = settings.value("ip", "127.0.0.1").toString();
    m_tcpClientPort = settings.value("port", "10000").toString();
    settings.endGroup();

    // 加载UDP配置
    settings.beginGroup("UDP");
    m_udpLocalPort = settings.value("localPort", "10001").toString();
    m_udpPeerIP = settings.value("peerIP", "127.0.0.1").toString();
    m_udpPeerPort = settings.value("peerPort", "10002").toString();
    settings.endGroup();

    // 加载显示配置
    settings.beginGroup("Display");
    m_hexShow = settings.value("hexShow", false).toBool();
    m_hexSend = settings.value("hexSend", false).toBool();
    m_autoClearSend = settings.value("autoClearSend", false).toBool();
    m_addTimestamp = settings.value("addTime", false).toBool();
    m_autoSend = settings.value("sendAuto", false).toBool();
    m_autoSendInterval = settings.value("autoSendTime", 1000).toInt();
    settings.endGroup();

    // 加载连接配置
    settings.beginGroup("General");
    m_connectionType = settings.value("connectionType", 0).toInt();
    settings.endGroup();

    // 加载快速发送条目
    settings.beginGroup("ExpPanel");
    int count = settings.value("rowCount", 0).toInt();
    m_quickSendEntries.clear();
    for (int i = 0; i < count; i++) {
        QVariantMap entry;
        entry["data"] = settings.value(QString("data_%1").arg(i)).toString();
        entry["hex"] = settings.value(QString("hex_%1").arg(i)).toBool();
        m_quickSendEntries.append(entry);
    }
    settings.endGroup();
}

// ==================== 串口设置器 ====================
void SettingsManager::setSerialBaudRateIndex(int index)
{
    if (m_serialBaudRateIndex != index) {
        m_serialBaudRateIndex = index;
        emit serialBaudRateIndexChanged();
    }
}

void SettingsManager::setSerialDataBits(int bits)
{
    if (m_serialDataBits != bits) {
        m_serialDataBits = bits;
        emit serialDataBitsChanged();
    }
}

void SettingsManager::setSerialStopBits(int bits)
{
    if (m_serialStopBits != bits) {
        m_serialStopBits = bits;
        emit serialStopBitsChanged();
    }
}

void SettingsManager::setSerialParity(int parity)
{
    if (m_serialParity != parity) {
        m_serialParity = parity;
        emit serialParityChanged();
    }
}

void SettingsManager::setSerialFlowControl(int flow)
{
    if (m_serialFlowControl != flow) {
        m_serialFlowControl = flow;
        emit serialFlowControlChanged();
    }
}

void SettingsManager::setManualPorts(const QStringList &ports)
{
    if (m_manualPorts != ports) {
        m_manualPorts = ports;
        emit manualPortsChanged();
    }
}

// ==================== TCP服务器设置器 ====================
void SettingsManager::setTcpServerPort(const QString &port)
{
    if (m_tcpServerPort != port) {
        m_tcpServerPort = port;
        emit tcpServerPortChanged();
    }
}

// ==================== TCP客户端设置器 ====================
void SettingsManager::setTcpClientIP(const QString &ip)
{
    if (m_tcpClientIP != ip) {
        m_tcpClientIP = ip;
        emit tcpClientIPChanged();
    }
}

void SettingsManager::setTcpClientPort(const QString &port)
{
    if (m_tcpClientPort != port) {
        m_tcpClientPort = port;
        emit tcpClientPortChanged();
    }
}

// ==================== UDP设置器 ====================
void SettingsManager::setUdpLocalPort(const QString &port)
{
    if (m_udpLocalPort != port) {
        m_udpLocalPort = port;
        emit udpLocalPortChanged();
    }
}

void SettingsManager::setUdpPeerIP(const QString &ip)
{
    if (m_udpPeerIP != ip) {
        m_udpPeerIP = ip;
        emit udpPeerIPChanged();
    }
}

void SettingsManager::setUdpPeerPort(const QString &port)
{
    if (m_udpPeerPort != port) {
        m_udpPeerPort = port;
        emit udpPeerPortChanged();
    }
}

// ==================== 显示设置器 ====================
void SettingsManager::setHexShow(bool show)
{
    if (m_hexShow != show) {
        m_hexShow = show;
        emit hexShowChanged();
    }
}

void SettingsManager::setHexSend(bool send)
{
    if (m_hexSend != send) {
        m_hexSend = send;
        emit hexSendChanged();
    }
}

void SettingsManager::setAutoClearSend(bool clear)
{
    if (m_autoClearSend != clear) {
        m_autoClearSend = clear;
        emit autoClearSendChanged();
    }
}

void SettingsManager::setAddTimestamp(bool add)
{
    if (m_addTimestamp != add) {
        m_addTimestamp = add;
        emit addTimestampChanged();
    }
}

void SettingsManager::setAutoSend(bool autoSend)
{
    if (m_autoSend != autoSend) {
        m_autoSend = autoSend;
        emit autoSendChanged();
    }
}

void SettingsManager::setAutoSendInterval(int interval)
{
    if (m_autoSendInterval != interval) {
        m_autoSendInterval = interval;
        emit autoSendIntervalChanged();
    }
}

// ==================== 连接设置器 ====================
void SettingsManager::setConnectionType(int type)
{
    if (m_connectionType != type) {
        m_connectionType = type;
        emit connectionTypeChanged();
    }
}

// ==================== 快速发送设置器 ====================
void SettingsManager::setQuickSendEntries(const QVariantList &entries)
{
    if (m_quickSendEntries != entries) {
        m_quickSendEntries = entries;
        emit quickSendEntriesChanged();
    }
}
