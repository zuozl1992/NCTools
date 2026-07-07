#pragma once

#include <QAbstractListModel>
#include <QVariantList>
#include <QVariantMap>

/**
 * @brief 快速发送数据模型
 *        管理快速发送面板的数据，支持20行快速发送条目
 *        每行包含数据内容和Hex模式标志
 */
class QuickSendModel : public QAbstractListModel
{
    Q_OBJECT

    // ==================== QML属性 ====================
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged) ///< 条目数量

public:
    /**
     * @brief 数据角色枚举
     */
    enum Roles {
        DataRole = Qt::UserRole + 1,  ///< 数据内容角色
        HexRole                        ///< Hex模式角色
    };

    /**
     * @brief 构造函数
     * @param rows 条目数量（默认20行）
     * @param parent 父对象
     */
    explicit QuickSendModel(int rows = 20, QObject *parent = nullptr);

    // ---- QAbstractListModel接口实现 ----
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    // ---- 条目操作 ----
    /**
     * @brief 设置条目数据
     * @param row 行索引
     * @param data 数据内容
     */
    Q_INVOKABLE void setEntryData(int row, const QString &data);

    /**
     * @brief 设置条目Hex模式
     * @param row 行索引
     * @param hex 是否为Hex模式
     */
    Q_INVOKABLE void setEntryHex(int row, bool hex);

    /**
     * @brief 获取条目数据
     * @param row 行索引
     * @return 数据内容
     */
    Q_INVOKABLE QString entryData(int row) const;

    /**
     * @brief 获取条目Hex模式
     * @param row 行索引
     * @return 是否为Hex模式
     */
    Q_INVOKABLE bool entryHex(int row) const;

    // ---- 批量操作 ----
    /**
     * @brief 从VariantList加载数据
     * @param entries 条目列表
     */
    Q_INVOKABLE void loadFromVariantList(const QVariantList &entries);

    /**
     * @brief 导出为VariantList
     * @return 条目列表
     */
    QVariantList toVariantList() const;

signals:
    void rowCountChanged();                               ///< 条目数量变更信号

private:
    /**
     * @brief 快速发送条目结构体
     */
    struct Entry {
        QString data;                                     ///< 数据内容
        bool hex = false;                                 ///< 是否为Hex模式
    };

    // ==================== 成员变量 ====================
    int m_rows;                                           ///< 条目总数
    QVector<Entry> m_entries;                             ///< 条目数据列表
};
