#include "foldersettingslistmodel.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>

FolderItem::FolderItem(const QString &name, const QString &path, const QString &iconName, const bool enabled):
    m_name(name),
    m_path(path),
    m_iconName(iconName),
    m_enabled(enabled) {}

QString FolderItem::name() const {
    return m_name;
}

QString FolderItem::path() const {
    return m_path;
}

QString FolderItem::iconName() const {
    return m_iconName;
}

bool FolderItem::enabled() const {
    return m_enabled;
}

void FolderItem::setName(const QString &newName) {
    if (m_name == newName)
        return;
    m_name = newName;

}

void FolderItem::setPath(const QString &newPath) {
    if (m_path == newPath)
        return;
    m_path = newPath;
}

void FolderItem::setIconName(const QString &newIconName) {
    if (m_iconName == newIconName)
        return;
    m_iconName = newIconName;
}

void FolderItem::setEnabled(const bool &newEnabled) {
    if (m_enabled == newEnabled)
        return;
    m_enabled = newEnabled;
}

QJsonObject FolderItem::toJson() const {
    QJsonObject json;
    json["name"] = m_name;
    json["path"] = m_path;
    json["iconName"] = m_iconName;
    json["enabled"] = m_enabled;
    return json;
}

FolderItem FolderItem::fromJson(const QJsonObject &json) {
    // return FolderItem(json["name"].toString(), json["path"].toString(), json["iconName"].toString(), json["enabled"].toBool());
    return {json["name"].toString(), json["path"].toString(), json["iconName"].toString(), json["enabled"].toBool()};
}


FolderSettingsListModel::FolderSettingsListModel(QObject *parent):
    QAbstractListModel(parent),
    m_showDisabled(true)
{
    refreshFolders();
    // const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/folders.json";
    //
    // if (QFile file(appDataPath); !file.exists()) {
    //     m_items = {
    //         FolderItem("Home", QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0], "folder-home", true),
    //         FolderItem("Documents", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "folder-documents", true),
    //         FolderItem("Downloads", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), "folder-downloads", true),
    //         FolderItem("Cache", QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation), "folder-cache", true),
    //     };
    //
    //     QJsonObject rootObject;
    //     auto foldersArray = QJsonArray();
    //     for (const auto &item : m_items) {
    //         foldersArray.append(item.toJson());
    //     }
    //     rootObject["folders"] = foldersArray;
    //     const QJsonDocument doc(rootObject);
    //     if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //         file.write(doc.toJson());
    //         file.close();
    //     }
    //
    // } else {
    //     if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //         const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    //         QJsonObject rootObject = doc.object();
    //         for (QJsonArray foldersArray = rootObject["folders"].toArray(); const auto &folder : foldersArray) {
    //             m_items.push_back(FolderItem::fromJson(folder.toObject()));
    //         }
    //     }
    //     else {
    //         qDebug() << "Error opening file";
    //     }
    // }

}

int FolderSettingsListModel::rowCount(const QModelIndex &parent) const {
    return static_cast<int>(m_items.size());
}

QVariant FolderSettingsListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};
    switch (role) {
        case NameRole:
            return m_items[index.row()].name();
        case PathRole:
            return m_items[index.row()].path();
        case IconNameRole:
            return m_items[index.row()].iconName();
        case EnabledRole:
            return m_items[index.row()].enabled();
        default:
            return {};
    }
}

QHash<int, QByteArray> FolderSettingsListModel::roleNames() const {
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {IconNameRole, "iconName"},
        {EnabledRole, "enabled"}
    };
}

bool FolderSettingsListModel::setData(const QModelIndex &index, const QVariant &value, const int role) {
    if (!index.isValid())
        return false;
    if (role == EnabledRole) {
        m_items[index.row()].setEnabled(value.toBool());
        emit dataChanged(index, index, {role});
        saveFolders();
        return true;
    }
    return false;
}

Qt::ItemFlags FolderSettingsListModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool FolderSettingsListModel::removeRows(const int row, const int count, const QModelIndex &parent) {
    if (row < 0 || row >= m_items.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    m_items.erase(m_items.begin() + row, m_items.begin() + row + count);
    saveFolders();
    endRemoveRows();
    return true;

}

bool FolderSettingsListModel::showDisabled() const {
    return m_showDisabled;
}

void FolderSettingsListModel::setShowDisabled(const bool &newShowDisabled) {
    if (m_showDisabled == newShowDisabled)
        return;
    m_showDisabled = newShowDisabled;
    emit showDisabledChanged();
    refreshFolders();
}


bool FolderSettingsListModel::setEnabled(int row, bool enabled) {
    if (row < 0 || row >= m_items.size())
        return false;

    return setData(index(row, 0), enabled, EnabledRole);

}

bool FolderSettingsListModel::remove(const int row) {
    return removeRow(row);
}

void FolderSettingsListModel::addFolder(const QString &name, const QString &path, const QString &iconName) {
    beginInsertRows(QModelIndex(), rowCount({}), rowCount({}));
    m_items.push_back(FolderItem(name, path, iconName, true));
    saveFolders();
    endInsertRows();

}

void FolderSettingsListModel::refreshFolders() {
    // qDebug() << "Refreshing folders" << sender() << this;
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/folders.json";
    beginRemoveRows({}, 0, rowCount({}) - 1);
    m_items.clear();
    endRemoveRows();

    if (QFile file(appDataPath); !file.exists()) {
        m_items = {
            FolderItem("Home", QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0], "folder-home", true),
            FolderItem("Documents", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "folder-documents", true),
            FolderItem("Downloads", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), "folder-downloads", true),
            FolderItem("Cache", QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation), "folder-cache", true),
        };
        beginInsertRows({}, 0, m_items.size() - 1);
        endInsertRows();

        QJsonObject rootObject;
        auto foldersArray = QJsonArray();
        for (const auto &item : m_items) {
            foldersArray.append(item.toJson());
        }
        rootObject["folders"] = foldersArray;
        const QJsonDocument doc(rootObject);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(doc.toJson());
            file.close();
        }

    } else {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject rootObject = doc.object();
            for (QJsonArray foldersArray = rootObject["folders"].toArray(); const auto &folder : foldersArray) {
                if (auto folderObject = folder.toObject(); m_showDisabled || folderObject["enabled"].toBool()) {
                    m_items.push_back(FolderItem::fromJson(folderObject));
                }
            }
            file.close();
            beginInsertRows({}, 0, m_items.size() - 1);
            endInsertRows();
        }
        else {
            qDebug() << "Error opening file";
        }
    }
}

void FolderSettingsListModel::saveFolders() {
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/folders.json";
    QJsonObject rootObject;
    auto foldersArray = QJsonArray();
    for (const auto &item : m_items) {
        foldersArray.append(item.toJson());
    }
    rootObject["folders"] = foldersArray;
    const QJsonDocument doc(rootObject);
    if (QFile file(appDataPath); file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
    }
}

