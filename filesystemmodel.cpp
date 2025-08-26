#include "filesystemmodel.h"
#include "core/helper.h"
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDatabase>
#include <QLocale> // For formatting numbers

//=============================================================================
// FileSystemModel Implementation
//=============================================================================

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractItemModel(parent), rootItem(QSharedPointer<TreeItem>(new TreeItem())) // Create root item
{
}

FileSystemModel::~FileSystemModel()
{
    // delete rootItem; // This will recursively delete all child TreeItems
    if (m_db.isOpen()) {
        m_db.close();
    }
}

// --- Database Initialization ---
bool FileSystemModel::initialize(const QString& dbPath)
{
    // Close existing connection if any
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE", "FileSystemConnection"); // Use a unique connection name
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "Error: Failed to connect to database:" << m_db.lastError().text();
        return false;
    }

    qInfo() << "Database connection established:" << dbPath;
    setupModelData(); // Load data from DB
    return true;
}

void FileSystemModel::deleteFile(const QString &pathath) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM files WHERE path = :path");
    query.bindValue(":path", pathath);
    query.exec();
    // setupModelData();
}


// --- QAbstractItemModel Overrides ---

QModelIndex FileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeItem *parentItem = getItem(parent); // Get parent TreeItem
    if (!parentItem)
        return {};

    TreeItem *childItem = parentItem->child(row).data(); // Get child TreeItem for the row
    if (childItem)
        // Create an index for the child, storing a pointer to it
        return createIndex(row, column, childItem);
    else
        return {}; // Should not happen if rowCount is correct
}

QModelIndex FileSystemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {}; // Invalid index has no parent

    TreeItem *childItem = getItem(index); // Get TreeItem for the given index
    if (!childItem) return {}; // Should not happen

    TreeItem *parentItem = childItem->parentItem(); // Get its parent TreeItem

    // If the parent is the root item, return an invalid index (as required by Qt)
    if (parentItem == rootItem)
        return {};

    // Otherwise, create an index for the parent
    return createIndex(parentItem->row(), 0, parentItem);
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    // Column doesn't affect row count for tree models
    if (parent.column() > 0)
        return 0;

    TreeItem *parentItem = getItem(parent); // Get parent TreeItem
    return parentItem ? parentItem->childCount() : 0;
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // We have two columns: Name and Size
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index); // Get the TreeItem for this index
    if (!item) return QVariant();

    // --- Role Handling ---
    switch (role) {
    case Qt::DisplayRole: // Default text data
        if (index.column() == 0){
            return item->name(); // Return file/folder name
        } else if (index.column() == 1){
            return Helper::formatSize(item->size());
        } else {
            return "";
        }
    case FilePathRole:
        return item->path(); // Return full file path

    // case NameRole:        // Explicit Name Role
    //     // return item->name(); // Return file/folder name
    //     if (index.column() == 0) {
    //         return item->name(); // Return file/folder name
    //     }
    //     else if (index.column() == 1) {
    //         // For DisplayRole in Size column, return formatted size
    //         // Return empty for folders unless you want to show aggregated size (more complex)
    //         // return formatSize(item->size());
    //         return "";
    //     }
    //     break;

    case SizeRole: // Explicit Formatted Size Role
        return Helper::formatSize(item->size());
        // if (index.column() == 1) {
        //     return formatSize(item->size());
        // }
        break;

    // case RawSizeRole: // Raw size for sorting
    //     // Return size for both files and folders (if needed for sorting folders)
    //     if (index.column() == 2){
    //         return QVariant::fromValue(item->size()); // Use qint64
    //     }
    //     break;

    // case IsFolderRole: // Role to check if it's a folder
    //     return item->isFolder();
    default:
        return "";

        // Add other roles as needed (e.g., Qt::DecorationRole for icons)
    }

    return QVariant(); // Return invalid variant for unhandled roles/columns
}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    // All items are enabled and selectable
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return QStringLiteral("Name");
        case 1: return QStringLiteral("Size");
        default: break;
        }
    }
    return {}; // Default implementation for other cases
}

// Expose role names to QML
QHash<int, QByteArray> FileSystemModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[FilePathRole] = "filePath";
    // roles[RawSizeRole] = "rawSize";
    // roles[IsFolderRole] = "isFolder";
    return roles;
}

bool FileSystemModel::removeRows(int row, int count, const QModelIndex &parent) {
    TreeItem *parentItem = getItem(parent);
    if (!parentItem || row < 0 || row + count > parentItem->childCount()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        TreeItem *childToRemove = parentItem->child(row).data();
        if (childToRemove) {
            // Remove from database if needed
            // deleteFile(childToRemove->path());

            auto childParent = childToRemove->parentItem();
            while (childParent) {
                childParent->changeSize(-childToRemove->size());
                childParent = childParent->parentItem();
            }

            parentItem->removeChild(row);
        }
    }


    endRemoveRows();
    return true;
}


// --- Private Helper Methods ---

// Fetches data from the database and builds the internal tree structure
void FileSystemModel::setupModelData()
{
    beginResetModel(); // Signal that the model is about to change drastically

    // Clear existing tree (delete children of root)
    rootItem = QSharedPointer<TreeItem>( new TreeItem());

    if (!m_db.isOpen()) {
        qWarning() << "Database is not open in setupModelData.";
        endResetModel();
        return;
    }

    QSqlQuery query(m_db);
    // Select all necessary columns
    if (!query.exec("SELECT id, path, size, is_folder, parent FROM files ORDER BY size DESC")) {
        qCritical() << "Failed to query files table:" << query.lastError().text();
        endResetModel();
        return;
    }

    QHash<int, TreeItem*> itemMap;
    itemMap.insert(0, rootItem.get()); // Assuming NULL parent ID maps to root

    // --- First Pass: Create all TreeItem objects ---
    while (query.next()) {
        int id = query.value("id").toInt();
        QString path = query.value("path").toString();
        qint64 size = query.value("size").toLongLong();
        bool isFolder = query.value("is_folder").toBool();
        int parentId = query.value("parent").isNull() ? 0 : query.value("parent").toInt();

        itemMap[parentId]->appendChild(new TreeItem(id, path, size, isFolder, nullptr));
        if (isFolder) {
            itemMap.insert(id, itemMap[parentId]->child(itemMap[parentId]->childCount()-1).data());
        }
    }

    endResetModel(); // Signal that the model has been reset
    qInfo() << "Model setup complete. Root item children:" << rootItem->childCount();
}


// Get the TreeItem associated with a QModelIndex
TreeItem *FileSystemModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        // Retrieve the pointer stored in the index
        auto *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    // If the index is invalid, it refers to the root item
    return rootItem.get();
}

// Helper function to format file sizes
// QString FileSystemModel::formatSize(qint64 bytes) const
// {
//     if (bytes < 0) return QString(); // Or handle error

//     const qint64 kilobyte = 1024;
//     const qint64 megabyte = kilobyte * 1024;
//     const qint64 gigabyte = megabyte * 1024;
//     const qint64 terabyte = gigabyte * 1024;

//     QLocale locale = QLocale::system(); // Use system locale for number formatting

//     if (bytes >= terabyte)
//         return locale.toString(static_cast<double>(bytes) / terabyte, 'f', 2) + " TB";
//     else if (bytes >= gigabyte)
//         return locale.toString(static_cast<double>(bytes) / gigabyte, 'f', 2) + " GB";
//     else if (bytes >= megabyte)
//         return locale.toString(static_cast<double>(bytes) / megabyte, 'f', 2) + " MB";
//     else if (bytes >= kilobyte)
//         return locale.toString(static_cast<double>(bytes) / kilobyte, 'f', 1) + " KB";
//     else
//         return locale.toString(bytes) + " Bytes";
// }
