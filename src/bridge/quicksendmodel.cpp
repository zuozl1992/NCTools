#include "quicksendmodel.h"

QuickSendModel::QuickSendModel(int rows, QObject *parent)
    : QAbstractListModel(parent)
    , m_rows(rows)
    , m_entries(rows)
{
}

int QuickSendModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_rows;
}

QVariant QuickSendModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows)
        return QVariant();

    const Entry &entry = m_entries[index.row()];
    switch (role) {
    case DataRole:
        return entry.data;    // 返回数据内容
    case HexRole:
        return entry.hex;     // 返回Hex模式标志
    default:
        return QVariant();
    }
}

bool QuickSendModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows)
        return false;

    Entry &entry = m_entries[index.row()];
    switch (role) {
    case DataRole:
        entry.data = value.toString();    // 设置数据内容
        break;
    case HexRole:
        entry.hex = value.toBool();       // 设置Hex模式标志
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> QuickSendModel::roleNames() const
{
    return {
        {DataRole, "entryData"},    // QML中的角色名称
        {HexRole, "entryHex"}
    };
}

void QuickSendModel::setEntryData(int row, const QString &data)
{
    if (row < 0 || row >= m_rows)
        return;

    m_entries[row].data = data;
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {DataRole});
}

void QuickSendModel::setEntryHex(int row, bool hex)
{
    if (row < 0 || row >= m_rows)
        return;

    m_entries[row].hex = hex;
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {HexRole});
}

QString QuickSendModel::entryData(int row) const
{
    if (row < 0 || row >= m_rows)
        return QString();
    return m_entries[row].data;
}

bool QuickSendModel::entryHex(int row) const
{
    if (row < 0 || row >= m_rows)
        return false;
    return m_entries[row].hex;
}

void QuickSendModel::loadFromVariantList(const QVariantList &entries)
{
    beginResetModel();
    for (int i = 0; i < entries.size() && i < m_rows; i++) {
        QVariantMap entry = entries[i].toMap();
        m_entries[i].data = entry["data"].toString();
        m_entries[i].hex = entry["hex"].toBool();
    }
    endResetModel();
}

QVariantList QuickSendModel::toVariantList() const
{
    QVariantList result;
    for (int i = 0; i < m_rows; i++) {
        QVariantMap entry;
        entry["data"] = m_entries[i].data;
        entry["hex"] = m_entries[i].hex;
        result.append(entry);
    }
    return result;
}
