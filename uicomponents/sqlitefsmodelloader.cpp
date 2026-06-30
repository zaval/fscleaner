#include "sqlitefsmodelloader.h"

#include <QSqlError>
#include <QSqlQuery>

QString SqliteFSModelLoader::createFilterTableQuery = R"(CREATE table filtered_files AS
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
WHERE ap.current_accumulated != '';)";

fscleanerPrivate::FilterTableThread::FilterTableThread(
    const QString &dbPath,
    const QString &filterText,
    QObject *parent
):
    QThread(parent),
    m_dbPath(dbPath),
    m_filterText(filterText)
{}

void fscleanerPrivate::FilterTableThread::run() {
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        qWarning() << "Failed to open database:"
                   << db.lastError().text()
                   << "Database path:"
                   << m_dbPath;
        return;
    }

    QSqlQuery query(db);
    query.exec("drop table if exists filtered_files");
    if (!query.exec()) {
        qWarning() << "Set filter query failure: "
               << query.lastError().text()
               << "Query:"
               << query.lastQuery();
        return;
    }
    query.prepare(SqliteFSModelLoader::createFilterTableQuery);
    const auto bindValue = QStringLiteral("%%1%").arg(m_filterText);
    query.addBindValue(bindValue);
    query.addBindValue(bindValue);
    if (!query.exec()) {
        qWarning() << "Set filter query failure: "
               << query.lastError().text()
               << "Query:"
               << query.lastQuery()
               << "Bound value:"
               << bindValue;
        return;
    }
    db.commit();
}

SqliteFSModelLoader::SqliteFSModelLoader(QObject *parent): BaseFSModelLoader(parent) {}

void SqliteFSModelLoader::loadChildren(FSTreeItem *parentItem) {
    if (parentItem->childrenLoaded) return;
    if (!m_db.isOpen()) {
        qWarning() << "Lazy load SQL query failure: database is not open"
            << "Database path:"
            << m_dbPath;

        return;
    }
    QSqlQuery query(m_db);

    if (m_filterByName.isEmpty()) {
        query.prepare("SELECT id, path, size, is_folder FROM files WHERE parent = ? ORDER BY size DESC");
    } else {
        query.prepare("SELECT id, path, size, is_folder FROM filtered_files WHERE parent = ? ORDER BY size DESC");
    }

    query.addBindValue(parentItem->id);

    if (!query.exec()) {
        qWarning() << "Lazy load SQL query failure:"
               << query.lastError().text()
               << "Query:"
               << query.lastQuery()
               << "Bound value:"
               << parentItem->id;
        return;
    }

    qDeleteAll(parentItem->children);
    parentItem->children.clear();
    int idx = 0;
    while (query.next()) {
        const int id = query.value(0).toInt();
        const QString path = query.value(1).toString();
        const qint64 size = query.value(2).toLongLong();
        const bool isFolder = query.value(3).toBool();

        auto* childItem = new FSTreeItem(id, path, size, isFolder, parentItem, idx++);
        parentItem->children.append(childItem);
    }
    parentItem->childrenLoaded = true;
}

void SqliteFSModelLoader::deleteItem(FSTreeItem *item) {
    QFile::remove(item->path);
}

void SqliteFSModelLoader::initialize(const QString &path) {
    m_dbPath = path;
    const QString connectionName =
        QStringLiteral("SqliteFSModelLoader_%1").arg(reinterpret_cast<quintptr>(this));

    if (m_db.isOpen()) {
        m_db.close();
    }

    m_db = QSqlDatabase();
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qWarning() << "Failed to open database:"
                   << m_db.lastError().text()
                   << "Database path:"
                   << path;
    }
}

void SqliteFSModelLoader::setFilter(const QString &filter) {
    if (m_filterByName == filter)
        return;
    m_filterByName = filter;

    if (!m_filterByName.isEmpty()) {
        auto* thread = new fscleanerPrivate::FilterTableThread(m_dbPath, m_filterByName, nullptr);
        connect(thread, &fscleanerPrivate::FilterTableThread::finished, thread, &QThread::deleteLater);
        connect(thread, &fscleanerPrivate::FilterTableThread::finished, this, [this]() {
            emit filterFinished();
        });
        thread->start();
    } else {
        emit filterFinished();
    }
}
