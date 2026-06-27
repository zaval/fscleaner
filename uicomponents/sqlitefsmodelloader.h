#ifndef FSCLEANER_SQLITEFSMODELLOADER_H
#define FSCLEANER_SQLITEFSMODELLOADER_H
#include <QSqlDatabase>
#include <QThread>

#include "basefsmodelloader.h"


namespace fscleanerPrivate {
    class FilterTableThread: public QThread {
        Q_OBJECT
    public:
        explicit FilterTableThread(const QString &dbPath, const QString &filterText, QObject *parent = nullptr);

    protected:
        void run() override;

    private:
        QString m_dbPath;
        QString m_filterText;
    };
}


class SqliteFSModelLoader: public BaseFSModelLoader {
    Q_OBJECT
    QML_ELEMENT

    friend class fscleanerPrivate::FilterTableThread;

public:
    explicit SqliteFSModelLoader(QObject *parent = nullptr);
    ~SqliteFSModelLoader() override = default;

public slots:
    void loadChildren(FSTreeItem *parentItem) override;
    void deleteItem(FSTreeItem *item) override;
    void initialize(const QString &path) override;
    void setFilter(const QString &filter) override;

private:
    QSqlDatabase m_db;
    QString m_dbPath;
    static QString createFilterTableQuery;
    QString m_filterByName;


};


#endif //FSCLEANER_SQLITEFSMODELLOADER_H
