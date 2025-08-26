//
// Created by Dmytrii Zavalnyi on 16.08.2025..
//

#ifndef FSCLEANER_ARCHIVEMODEL_H
#define FSCLEANER_ARCHIVEMODEL_H
#include <QAbstractItemModel>
#include <QtQmlIntegration/qqmlintegration.h>
#include <KArchive>


class ArchiveModel: public QAbstractItemModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString archiveFileName READ archiveFileName WRITE setArchiveFileName NOTIFY archiveFileNameChanged)
    Q_PROPERTY(bool isLoading READ isLoading WRITE setIsLoading NOTIFY isLoadingChanged)

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QHash<int, QByteArray> roleNames() const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QString archiveFileName() const;

    void setArchiveFileName(const QString &newArchiveFileName);

    bool isLoading() const;
    void setIsLoading(const bool &newIsLoading);

    signals:
        void archiveFileNameChanged();
    void isLoadingChanged();
private:
    std::unique_ptr<KArchive> m_archive;

    QString m_archiveFileName;
    bool m_isLoading = false;

    struct TreeItem {
        const KArchiveEntry *entry = nullptr;
        TreeItem* parent = nullptr;
        std::vector<std::unique_ptr<TreeItem>> children = {};
        int64_t size = 0;
        int64_t calculateSize() {
            int64_t sz = 0;
            if (entry->name() == "org") {
                const auto a = 12;
            }
            if (entry->isDirectory()) {

                for (const auto &child: children) {
                    sz += child->calculateSize();
                    child->size = sz;
                }
            } else {
                sz = dynamic_cast<const KArchiveFile*>(entry)->size();
            }

            return sz;
        }
    };

    std::unique_ptr<TreeItem> m_root;

    void processArchive();
    void loadArchiveFolder(TreeItem *dir);

};


#endif //FSCLEANER_ARCHIVEMODEL_H