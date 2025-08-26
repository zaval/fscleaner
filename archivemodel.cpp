//
// Created by Dmytrii Zavalnyi on 16.08.2025..
//

#include "archivemodel.h"
#include <memory>

#include "kar.h"
#include "KTar"
#include "KZip"
#include "core/helper.h"

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return {};

    const auto *item = static_cast<TreeItem*>(parent.internalPointer());

    if (!item) {
        item = m_root.get();
    }

    if (row < 0 || row >= item->children.size()) {
        return {};
    }

    return createIndex(row, column, item->children[row].get());
}

int ArchiveModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2;
}

QHash<int, QByteArray> ArchiveModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    // roles[Qt::UserRole + 1] = "size";
    return roles;
}


QModelIndex ArchiveModel::parent(const QModelIndex &child) const {
    const auto *childItem = static_cast<TreeItem*>(child.internalPointer());
    if (!childItem) {
        return {};
    }
    if (childItem == m_root.get()) {
        return {};
    }
    auto *parentItem = childItem->parent;
    if (!parentItem) {
        return {};
    }
    return createIndex(child.row(), child.column(), parentItem);
}

int ArchiveModel::rowCount(const QModelIndex &parent) const {
    // if (!parent.isValid())
    //     return 0;

    if (m_root == nullptr) {
        return 0;
    }
    if (parent.column() > 1)
        return 0;
    auto *item = static_cast<TreeItem*>(parent.internalPointer());
    if (!item) {
        item = m_root.get();
    }

    return item->children.size();
}

QVariant ArchiveModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    auto *item = static_cast<TreeItem*>(index.internalPointer());
    if (!item) {
        return {};
    }

    if (index.column() == 0) {
        return item->entry->name();
    } else {
        // return item->size;
        return Helper::formatSize(item->size);
        // return item->entry->isFile() ?  dynamic_cast<const KArchiveFile*>(item->entry)->size() : calculateFolderSize(item);
    }
}

QVariant ArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return QStringLiteral("Name");
            case 1: return QStringLiteral("Size");
            default: break;
        }
    }
    return {};
}

Qt::ItemFlags ArchiveModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QString ArchiveModel::archiveFileName() const {
    return m_archiveFileName;
}

void ArchiveModel::setArchiveFileName(const QString &newArchiveFileName) {
    if (m_archiveFileName == newArchiveFileName)
        return;
    m_archiveFileName = newArchiveFileName;
    setIsLoading(true);
    processArchive();
    setIsLoading(false);

    emit archiveFileNameChanged();

}

bool ArchiveModel::isLoading() const {
    return m_isLoading;
}

void ArchiveModel::setIsLoading(const bool &newIsLoading) {
    if (m_isLoading == newIsLoading)
        return;
    m_isLoading = newIsLoading;
    emit isLoadingChanged();

}

void ArchiveModel::processArchive() {
    if (m_archiveFileName.isEmpty()) {
        return;
    }
    if (
        m_archiveFileName.endsWith(".zip", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".jar", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".apk", Qt::CaseInsensitive)
        ) {
        m_archive = std::make_unique<KZip>(m_archiveFileName);
    } else if (m_archiveFileName.endsWith(".ar", Qt::CaseInsensitive)) {
        m_archive = std::make_unique<KAr>(m_archiveFileName);
    } else if (
        m_archiveFileName.endsWith(".tar", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tar.gz", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tar.bz2", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tar.xz", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tbz", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tgz", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".txz", Qt::CaseInsensitive) ||
        m_archiveFileName.endsWith(".tar.bz", Qt::CaseInsensitive)
        ) {
        m_archive = std::make_unique<KTar>(m_archiveFileName);
    } else {
        qDebug() << "Unknown archive format" << m_archiveFileName;
        return;
    }


    if (!m_archive->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open archive";
        return;
    }
    const auto *dir = m_archive->directory();
    beginResetModel();
    m_root = std::make_unique<TreeItem>();

    for (const auto &entry : dir->entries()) {
        const auto *e = dir->entry(entry);
        // auto *rawItem = new TreeItem{e, m_root.get()};
        std::unique_ptr<TreeItem> treeItem(new TreeItem{e, m_root.get()});
        if (e->isDirectory()) {
            loadArchiveFolder(treeItem.get());
        }
        m_root->children.push_back(std::move(treeItem));


    }
    for (const auto &child: m_root->children) {
        child->size = child->calculateSize();
    }
    endResetModel();
}

void ArchiveModel::loadArchiveFolder(TreeItem *dir) {
    const auto *dirEntry = dynamic_cast<const KArchiveDirectory*>(dir->entry);
    for (const auto &entry : dirEntry->entries()) {
        const auto *e = dirEntry->entry(entry);
        // auto *rawItem = new TreeItem{e, dir};

        std::unique_ptr<TreeItem> treeItem(new TreeItem{e, dir});
        if (e->isDirectory()) {
            loadArchiveFolder(treeItem.get());
        }
        dir->children.push_back(std::move(treeItem));
    }
}
