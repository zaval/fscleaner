#ifndef FSCLEANER_FSMODEL_H
#define FSCLEANER_FSMODEL_H
#include <QAbstractItemModel>
#include <QMimeDatabase>
#include <QSqlDatabase>
#include <QThread>
#include <QtQmlIntegration/qqmlintegration.h>
#include "basefsmodelloader.h"


class FSModel: public QAbstractItemModel {
    Q_OBJECT

    Q_PROPERTY(QString filterByName READ filterByName WRITE setFilterByName NOTIFY filterByNameChanged)
    Q_PROPERTY(bool isLoading READ isLoading WRITE setIsLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(BaseFSModelLoader* modelLoader READ modelLoader WRITE setModelLoader NOTIFY modelLoaderChanged)

    QML_ELEMENT

public:
    enum Roles {
        FilePathRole = Qt::UserRole + 1,
        SizeRole, // Formatted size string
        IconRole,
        IdRole,
    };
    explicit FSModel(QObject *parent = nullptr);
    ~FSModel() override;

    int rowCount(const QModelIndex &parent) const override; // NOLINT(*-use-nodiscard)

    int columnCount(const QModelIndex &parent) const override; // NOLINT(*-use-nodiscard)

    bool hasChildren(const QModelIndex &parent) const override; // NOLINT(*-use-nodiscard)

    QHash<int, QByteArray> roleNames() const override; // NOLINT(*-use-nodiscard)

    QVariant data(const QModelIndex &index, int role) const override; // NOLINT(*-use-nodiscard)

    Qt::ItemFlags flags(const QModelIndex &index) const override; // NOLINT(*-use-nodiscard)

    QModelIndex parent(const QModelIndex &child) const override; // NOLINT(*-use-nodiscard)

    QModelIndex index(int row, int column, const QModelIndex &parent) const override; // NOLINT(*-use-nodiscard)

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override; // NOLINT(*-use-nodiscard)

    Q_INVOKABLE bool initialize(const QString& dbPath);

    QString filterByName() const; // NOLINT(*-use-nodiscard)

    void setFilterByName(const QString &newFilterByName);

    static QString createFilterTableQuery;

    bool isLoading() const; // NOLINT(*-use-nodiscard)

    void setIsLoading(const bool &newIsLoading);

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    BaseFSModelLoader* modelLoader() const; // NOLINT(*-use-nodiscard)

    void setModelLoader(BaseFSModelLoader* newModelLoader);

    Q_INVOKABLE bool removeIndex(const QModelIndex &index);




signals:
    void filterByNameChanged();
    void isLoadingChanged();
    void modelLoaderChanged();


private:
    FSTreeItem* rootItem;
    // FileTypeIcon m_fileTypeIcon;
    void loadChildren(FSTreeItem* parentItem);
    QString m_dbPath;
    QSqlDatabase m_db;
    QMimeDatabase m_mimeDB;
    QString getFileIcon(const QString& path) const; // NOLINT(*-use-nodiscard)
    QString m_filterByName;

    bool m_isLoading;
    BaseFSModelLoader* m_modelLoader;



};

#endif //FSCLEANER_FSMODEL_H
