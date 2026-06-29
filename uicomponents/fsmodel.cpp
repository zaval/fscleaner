#include "fsmodel.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "core/helper.h"

QString FSModel::createFilterTableQuery = R"(
CREATE table filtered_files AS
WITH RECURSIVE accumulated_paths AS (
    SELECT path,
           path AS remaining_path,
           path AS current_accumulated
    FROM files
    WHERE path LIKE ?

    UNION ALL

    SELECT path,
           path AS remaining_path,
           ''   AS current_accumulated
    FROM files
    WHERE path LIKE ?

    UNION ALL

    SELECT path,
           SUBSTR(remaining_path, INSTR(remaining_path, '/') + 1) AS remaining_path,
           CASE
               WHEN current_accumulated = '' THEN SUBSTR(remaining_path, 1, INSTR(remaining_path, '/'))
               ELSE current_accumulated || SUBSTR(remaining_path, 1, INSTR(remaining_path, '/'))
               END AS current_accumulated
    FROM accumulated_paths
    WHERE current_accumulated != path
      AND INSTR(remaining_path, '/') > 0
)
SELECT DISTINCT f.*
FROM accumulated_paths ap
         JOIN files f ON f.path = CASE
                                      WHEN ap.current_accumulated = '/' THEN '/'
                                      ELSE RTRIM(ap.current_accumulated, '/')
    END
WHERE ap.current_accumulated != '';
)";

bool FSModel::isLoading() const {
    return m_isLoading;
}

void FSModel::setIsLoading(const bool &newIsLoading) {
    if (m_isLoading == newIsLoading)
        return;
    m_isLoading = newIsLoading;
    emit isLoadingChanged();

}

bool FSModel::removeRows(const int row, const int count, const QModelIndex &parent) {
    auto parentItem = static_cast<FSTreeItem *>(parent.internalPointer());

    if (!parentItem || row < 0 || row + count > parentItem->children.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        if (m_modelLoader) {
            m_modelLoader->deleteItem(parentItem->children.at(row));
        }
        delete parentItem->children.at(row);
    }
    parentItem->children.erase(parentItem->children.begin() + row, parentItem->children.begin() + row + count);
    endRemoveRows();
    return true;
}

BaseFSModelLoader * FSModel::modelLoader() const {
    return m_modelLoader;
}

void FSModel::setModelLoader(BaseFSModelLoader *newModelLoader) {
    if (m_modelLoader == newModelLoader)
        return;
    m_modelLoader = newModelLoader;
    connect(m_modelLoader, &BaseFSModelLoader::filterFinished, this, [this]() {
        // beginResetModel();
        // endResetModel();
            initialize(m_dbPath);
        setIsLoading(false);
    });
    m_modelLoader->initialize(m_dbPath);
    initialize(m_dbPath);
    emit modelLoaderChanged();

}

bool FSModel::removeIndex(const QModelIndex &index) {
    if (!index.isValid())
        return false;

    return removeRows(index.row(), 1, index.parent());
}


FSModel::FSModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_isLoading(false),
    m_modelLoader(nullptr)
{
    rootItem = new FSTreeItem(0, "", 0, true);
}

FSModel::~FSModel() {
    delete rootItem;
    const QString connectionName = m_db.connectionName();

    if (m_db.isOpen()) {
        m_db.close();
    }

    m_db = QSqlDatabase();

    if (!connectionName.isEmpty() && QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

int FSModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) {
        return 0;
    }

    FSTreeItem* parentItem = !parent.isValid() ? rootItem : static_cast<FSTreeItem*>(parent.internalPointer());

    if (!parentItem->isFolder) {
        return 0;
    }

    if (!parentItem->childrenLoaded) {
        const_cast<FSModel*>(this)->loadChildren(parentItem);
    }

    return static_cast<int>(parentItem->children.size());
}

int FSModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

bool FSModel::hasChildren(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return true;
    }
    const auto* item = static_cast<FSTreeItem*>(parent.internalPointer());
    return item->isFolder;
}

QVariant FSModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    auto* item = static_cast<FSTreeItem*>(index.internalPointer());

    switch (role) {
        case FilePathRole:
            return item->path;
        case SizeRole:
            return Helper::formatSize(item->size);
        case Qt::DisplayRole:
            if (index.column() == 0){
                return item->name; // Return file/folder name
            } else if (index.column() == 1){
                return Helper::formatSize(item->size);
            } else {
                return "";
            }
        case IconRole:
            if (index.column() == 0) {
                if (item->isFolder) {
                    return "folder";
                } else {
                    // return m_fileTypeIcon.getIcon(item->path);
                    return getFileIcon(item->path);
                }
            } else {
                return "";
            }
        case IdRole:
            return item->id;
        default:
            return {};
    }
}

QHash<int, QByteArray> FSModel::roleNames() const {
    return {
        {Qt::DisplayRole, "display"},
        {FilePathRole, "filePath"},
        {IconRole, "icon"},
        {SizeRole, "size"},
        {IdRole, "itemId"},
    };
}

Qt::ItemFlags FSModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    // All items are enabled and selectable
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex FSModel::parent(const QModelIndex &child) const {
    if (!child.isValid()) {
        return {};
    }

    const auto childItem = static_cast<FSTreeItem*>(child.internalPointer());
    const auto parentItem = childItem->parentItem;

    if (parentItem == rootItem || !parentItem) {
        return {};
    }

    return createIndex(parentItem->rowInParent, 0, parentItem);
}

QModelIndex FSModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    FSTreeItem* parentItem = !parent.isValid() ? rootItem : static_cast<FSTreeItem*>(parent.internalPointer());

    if (!parentItem->childrenLoaded) {
        const_cast<FSModel*>(this)->loadChildren(parentItem);
    }

    FSTreeItem* childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return {};
}


QVariant FSModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return QStringLiteral("Name");
            case 1: return QStringLiteral("Size");
            default: break;
        }
    }
    return {}; // Default implementation for other cases
}

bool FSModel::initialize(const QString &dbPath) {
    m_dbPath = dbPath;

    beginResetModel();

    qDeleteAll(rootItem->children);
    rootItem->children.clear();
    rootItem->childrenLoaded = false;
    if (m_modelLoader) {
        m_modelLoader->initialize(m_dbPath);
    }
    endResetModel();
    return true;
}

QString FSModel::filterByName() const {
    return m_filterByName;
}

void FSModel::setFilterByName(const QString &newFilterByName) {
    if (m_filterByName == newFilterByName)
        return;
    m_filterByName = newFilterByName;
    emit filterByNameChanged();
    if (m_modelLoader) {
        setIsLoading(true);
        m_modelLoader->setFilter(m_filterByName);
    }
}

void FSModel::loadChildren(FSTreeItem *parentItem) {
    if (parentItem->childrenLoaded) return;
    if (m_modelLoader) {
        m_modelLoader->loadChildren(parentItem);
    }
}

QString FSModel::getFileIcon(const QString &path) const {
    const QMimeType mime = m_mimeDB.mimeTypeForFile(path);
    // qDebug() << "mime type:" << mime.name();
    if (mime.inherits("text/plain")) {
        return "file-text";
    }
    if (mime.name().startsWith("image/")) {
        return "file-image";
    }
    if (mime.name().startsWith("audio/")) {
        return "file-music";
    }
    if (mime.name().startsWith("video/")) {
        return "file-video";
    }
    if (
        mime.inherits("application/zip") ||
        mime.inherits("application/zlib") ||
        mime.inherits("application/x-bzip2") ||
        mime.inherits("application/x-tar") ||
        mime.inherits("application/gzip")
    ) {
        return "file-archive";
    }
    return "file";
}
