#include "archivefsmodelloader.h"
#include <KArchive>
#include <QDir>

#include "kar.h"
#include "ktar.h"
#include "kzip.h"

ArchiveFSModelLoader::ArchiveFSModelLoader(QObject *parent): BaseFSModelLoader(parent) {
}

void ArchiveFSModelLoader::extract(const QString &archivePath, const QString &extractPath) {
    if (archivePath.isEmpty() || extractPath.isEmpty()) {
        qDebug() << "Invalid archive path" << archivePath << " or extract path: " << extractPath;
        return;
    }
    const auto normalizedPath = archivePath.startsWith("/") ? archivePath.last(archivePath.length()-1) : archivePath;
    const auto dir = m_archive->directory();
    const auto entry = dir->entry(normalizedPath);
    if (!entry) {
        qDebug() << "Could not find directory" << normalizedPath;
        return;
    }
    if (entry->isFile()) {
        const auto f = dynamic_cast<const KArchiveFile*>(entry);
        if (!f) {
            qDebug() << "Unknown archive format" << normalizedPath;
            return;
        }
        QFile outputFile(extractPath + QDir::separator() + f->name());
        if (outputFile.open(QIODevice::WriteOnly)) {
            std::unique_ptr<QIODevice> device(f->createDevice());
            QDataStream in(device.get());
            QDataStream out(&outputFile);
            constexpr int chunkSize = 4096;
            char buffer[chunkSize];
            while (!device->atEnd()) {
                const auto bytesRead = in.readRawData(buffer, chunkSize);
                if (bytesRead <= 0) break;

                const auto bytesWritten = out.writeRawData(buffer, bytesRead);
                if (bytesWritten < 0) return;
            }
        }
    }
    if (entry->isDirectory()) {
        const auto childDir = dynamic_cast<const KArchiveDirectory*>(entry);
        if (!childDir) {
            qDebug() << "Unknown archive format" << normalizedPath;
            return;
        }

        const auto created = QDir(extractPath).mkdir(childDir->name());
        if (!created) {
            qDebug() << "Could not create directory" << extractPath << QDir::separator() << childDir->name();
            return;
        }

        for (const auto &itemName: childDir->entries()) {
            extract(normalizedPath + QDir::separator() + itemName, extractPath + QDir::separator() + childDir->name());
        }
    }
}

void ArchiveFSModelLoader::loadChildren(FSTreeItem *parentItem) {
    if (!m_archive) {
        return;
    }
    if (parentItem->childrenLoaded) return;

    qDeleteAll(parentItem->children);
    parentItem->children.clear();

    const auto dir = m_archive->directory();
    const auto entry = parentItem->path.isEmpty() ? dir : dynamic_cast<const KArchiveDirectory*>(dir->entry(parentItem->path));

    if (!entry) {
        parentItem->childrenLoaded = true;
        return;
    }

    int idx = 0;
    for (const auto &itemName: entry->entries()) {
        const auto item = entry->entry(itemName);
        const auto path = parentItem->path + (parentItem->path.isEmpty() ? "" : "/") + itemName;
        if (!m_filteredFiles.isEmpty() && !m_filteredFiles.contains(path)) {
            continue;
        }
        if (item->isFile()) {
            const auto f = dynamic_cast<const KArchiveFile*>(item);
            qint64 sz = 0;
            if (f) {
                sz = f->size();
            }
            auto* childItem = new FSTreeItem(0, path, sz, false, parentItem, idx++);
            parentItem->children.append(childItem);
        }

        if (item->isDirectory()) {
            const auto d = dynamic_cast<const KArchiveDirectory*>(item);
            auto* childItem = new FSTreeItem(0, path, getDirectorySize(d), true, parentItem, idx++);
            parentItem->children.append(childItem);
        }
    }
    std::ranges::sort(parentItem->children, [](const auto &a, const auto &b) { return a->size > b->size; });
    parentItem->childrenLoaded = true;
}

void ArchiveFSModelLoader::deleteItem(FSTreeItem *item) {
}

void ArchiveFSModelLoader::initialize(const QString &path) {
    if (path.isEmpty()) return;

    const auto mime = m_mimeDB.mimeTypeForFile(path);

    if (mime.inherits("application/zip")) {
        m_archive = std::make_unique<KZip>(path);
    } else if (path.endsWith(".ar", Qt::CaseInsensitive)) {
        m_archive = std::make_unique<KAr>(path);
    } else if (
        mime.inherits("application/x-tar") ||
        mime.inherits("application/x-xz") ||
        mime.inherits("application/x-bzip2")
    ) {
        m_archive = std::make_unique<KTar>(path);
    } else {
            qDebug() << "Unknown archive format" << path;
            return;
    }

    m_archive->open(QIODevice::ReadOnly);
}

void ArchiveFSModelLoader::setFilter(const QString &filter) {
    m_filteredFiles.clear();
    if (filter.isEmpty()) {
        emit filterFinished();
        return;
    }
    if (!m_archive) {
        return;
    }

    loadFilteredFiles(filter, "");

    // for (const auto &f: m_filteredFiles) {
    //     qDebug() << f;
    // }
    emit filterFinished();

}

qint64 ArchiveFSModelLoader::getDirectorySize(const KArchiveDirectory *dir) const {
    if (!m_archive) {
        return 0;
    }
    if (!dir) {
        return 0;
    }
    auto size = 0;

    for (const auto &itemName: dir->entries()) {
        const auto item = dir->entry(itemName);
        if (item->isDirectory()) {
            const auto childDir = dynamic_cast<const KArchiveDirectory*>(item);
            size += getDirectorySize(childDir);
        }
        if (item->isFile()) {
            const auto childFile = dynamic_cast<const KArchiveFile*>(item);
            size += childFile->size();
        }
    }

    return size;
}

bool ArchiveFSModelLoader::loadFilteredFiles(const QString &filter, const QString &folder) {
    const auto dir = m_archive->directory();
    // const auto entry = dir->entry(folder);
    bool res = false;
    const auto entry = folder.isEmpty() ? dir : dir->entry(folder);
    if (!entry) {
        return false;
    }
    if (entry->isDirectory()) {
        const auto childDir = dynamic_cast<const KArchiveDirectory*>(entry);
        for (const auto &itemName: childDir->entries()) {
            res = loadFilteredFiles(filter, folder + (folder.isEmpty() ? "" : "/") + itemName);
            if (res) {
                m_filteredFiles.append(folder);

            }
        }
        return res;
    }
    if (entry->isFile()) {
        const auto childFile = dynamic_cast<const KArchiveFile*>(entry);
        if (childFile->name().contains(filter, Qt::CaseInsensitive)) {
            m_filteredFiles.append(folder);
            return true;
        } else {
            return false;
        }
    }
    return false;
}
