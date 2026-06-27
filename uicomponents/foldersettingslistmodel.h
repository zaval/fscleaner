#ifndef FSCLEANER_FOLDERSETTINGSLISTMODEL_H
#define FSCLEANER_FOLDERSETTINGSLISTMODEL_H
#include <QAbstractListModel>
#include <qqmlintegration.h>


class FolderItem {
public:
    FolderItem(const QString &name, const QString &path, const QString &iconName, bool enabled);
    QString name() const;
    QString path() const;
    QString iconName() const;
    bool enabled() const;

    void setName(const QString &newName);
    void setPath(const QString &newPath);
    void setIconName(const QString &newIconName);
    void setEnabled(const bool &newEnabled);

    QJsonObject toJson() const;
    static FolderItem fromJson(const QJsonObject &json);

private:
    QString m_name;
    QString m_path;
    QString m_iconName;
    bool m_enabled;
};

class FolderSettingsListModel: public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(bool showDisabled READ showDisabled WRITE setShowDisabled NOTIFY showDisabledChanged)
    QML_ELEMENT

public:
    enum ProjectRoles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        IconNameRole,
        EnabledRole
    };

    explicit FolderSettingsListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    bool showDisabled() const;

    void setShowDisabled(const bool &newShowDisabled);


    Q_INVOKABLE bool setEnabled(int row, bool enabled);
    Q_INVOKABLE bool remove(int row);
    Q_INVOKABLE void addFolder(const QString &name, const QString &path, const QString &iconName);
    Q_INVOKABLE void refreshFolders();

signals:
    void showDisabledChanged();

private:
    QList<FolderItem> m_items;
    void saveFolders();
    bool m_showDisabled;
};


#endif //FSCLEANER_FOLDERSETTINGSLISTMODEL_H
