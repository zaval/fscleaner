#include "uihelper.h"

#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include "version.h"

QString UIHelper::version() const {
    return FSCleanerVersion::string;
}

QString UIHelper::homeDirectory() const { return QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0]; }

QString UIHelper::documentsDirectory() const {
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0];
}

QString UIHelper::downloadsDirectory() const {
    return QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)[0];
}

QString UIHelper::cacheDirectory() const {
    return QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation)[0];
}

QString UIHelper::getFolderName(const QString &path) const {
    const QDir dir(path);
    return dir.dirName();
}

QString UIHelper::getFileName(const QString &path) const {
    const QFileInfo fileInfo(path);
    return fileInfo.fileName();
}

bool UIHelper::remove(const QString &path) {
    const QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        return false;
    }
    if (fileInfo.isDir()) {
        return QDir(path).removeRecursively();
    } else {
        return QFile::remove(path);
    }

}


