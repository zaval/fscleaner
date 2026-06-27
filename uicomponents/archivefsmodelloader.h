#ifndef FSCLEANER_ARCHIVEFSMODELLOADER_H
#define FSCLEANER_ARCHIVEFSMODELLOADER_H
#include <QMimeDatabase>

#include "basefsmodelloader.h"
#include <KArchive>


// class KArchive;

class ArchiveFSModelLoader: public BaseFSModelLoader {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ArchiveFSModelLoader(QObject *parent = nullptr);
    ~ArchiveFSModelLoader() override = default;

    Q_INVOKABLE void extract(const QString& archivePath, const QString& extractPath);

public slots:
    void loadChildren(FSTreeItem *parentItem) override;

    void deleteItem(FSTreeItem *item) override;

    void initialize(const QString &path) override;

    void setFilter(const QString &filter) override;

private:
    std::unique_ptr<KArchive> m_archive;
    QMimeDatabase m_mimeDB;
    qint64 getDirectorySize(const KArchiveDirectory *dir) const;
    QStringList m_filteredFiles;
    QSet<QString> m_filteredFilesSet;
    bool loadFilteredFiles(const QString &filter, const QString &folder="");
};


#endif //FSCLEANER_ARCHIVEFSMODELLOADER_H
