#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QDebug>
#include <QFileInfo>
#include <QVariant>
#include <QVector>
#include <QHash>
#include <QSharedPointer>
#include <QtQmlIntegration/qqmlintegration.h>



// Forward declaration
class TreeItem;
using TreeItemPtr = QSharedPointer<TreeItem>;

//=============================================================================
// FileSystemModel Class
//=============================================================================
class FileSystemModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    // Enum for custom roles used by QML delegates and sorting
    enum Roles {
        FilePathRole = Qt::UserRole + 1,
        // NameRole = Qt::UserRole + 1,
        SizeRole,       // Formatted size string
        // RawSizeRole,    // Integer size for sorting
        // IsFolderRole
    };


    explicit FileSystemModel(QObject *parent = nullptr);
    ~FileSystemModel() override;

    // --- QAbstractItemModel overrides ---
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override; // Expose roles to QML
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    // --- Public Methods ---
    // Initializes the model by connecting to the DB and fetching data
    Q_INVOKABLE bool initialize(const QString& dbPath);
    Q_INVOKABLE void deleteFile(const QString& pathath);

private:
    // --- Private Helper Methods ---
    void setupModelData(); // Fetches data and builds the tree
    TreeItem *getItem(const QModelIndex &index) const; // Get TreeItem from index
    // QString formatSize(qint64 size) const; // Helper to format size

    // --- Member Variables ---
    // TreeItem *rootItem;       // Root of the internal tree structure
    QSharedPointer<TreeItem> rootItem;       // Root of the internal tree structure
    QSqlDatabase m_db;        // Database connection
};


//=============================================================================
// TreeItem Class (Internal Helper)
//=============================================================================
class TreeItem
{
public:
    // Constructor for the root item
    explicit TreeItem(TreeItem *parent = nullptr) : m_parentItem(parent) {}

    // Constructor for data items
    explicit TreeItem(int id, const QString &path, qint64 size, bool isFolder, TreeItem *parent = nullptr)
        : m_id(id), m_path(path), m_size(size), m_isFolder(isFolder), m_parentItem(parent) {}

    ~TreeItem() = default;

    // ~TreeItem() {
    //     qDeleteAll(m_childItems); // Recursively delete children
    // }

    void appendChild(const QSharedPointer<TreeItem>& child) {
        child->m_parentItem = this;
        m_childItems.append(child);
    }

    // --- Child Management ---
    void appendChild(TreeItem *child) {
        child->m_parentItem = this;
        m_childItems.append(QSharedPointer<TreeItem>(child));
        // child->m_parentItem = sharedFromThis();
        // m_childItems.append(child);

    }

    // QSharedPointer<TreeItem> lastChild(int row) {}

    // TreeItem *child(int row) {
    QSharedPointer<TreeItem> child(int row) {
        if (row < 0 || row >= m_childItems.size())
            return nullptr;
        return m_childItems.at(row);
    }

    int childCount() const {
        return m_childItems.count();
    }

    // --- Data Access ---
    int id() const { return m_id; }
    QString path() const { return m_path; }
    QString name() const { return QFileInfo(m_path).fileName(); } // Extract name from path
    qint64 size() const { return m_size; }
    bool isFolder() const { return m_isFolder; }
    void changeSize(qint64 newSize) { m_size = m_size + newSize; }
    void removeChild(int row) {
        if (row < 0 || row >= m_childItems.size())
            return;
        m_childItems.removeAt(row);
    }

    // --- Tree Structure ---
    int row() const {
        if (m_parentItem)
            return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
        return 0; // Root item is at row 0 of its (non-existent) parent
    }

    TreeItem *parentItem() {
        return m_parentItem;
    }

    TreeItem *m_parentItem;          // Pointer to parent
private:
    int m_id = -1;
    QString m_path;
    qint64 m_size = 0;
    bool m_isFolder = false;

    // QVector<TreeItem*> m_childItems; // Pointers to children
    QVector<QSharedPointer<TreeItem>> m_childItems; // Pointers to children

};


#endif // FILESYSTEMMODEL_H
