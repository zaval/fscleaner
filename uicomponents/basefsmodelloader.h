#ifndef FSCLEANER_BASEFSMODELLOADER_H
#define FSCLEANER_BASEFSMODELLOADER_H
#include <QFileInfo>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

struct FSTreeItem {
    int id;
    QString path;
    QString name;
    qint64 size;
    bool isFolder;
    FSTreeItem* parentItem;
    int rowInParent;
    QList<FSTreeItem*> children;
    bool childrenLoaded;

    FSTreeItem(const int id, const QString &path, const qint64 size, const bool isFolder, FSTreeItem* parent = nullptr, const int row = 0)
        : id(id), path(path), size(size), isFolder(isFolder), parentItem(parent), rowInParent(row), childrenLoaded(false) {
        name = QFileInfo(path).fileName();
    }

    ~FSTreeItem() {
        qDeleteAll(children);
    }

    FSTreeItem* child(int row) const { // NOLINT(*-use-nodiscard)
        if (row >= 0 && row < children.size()) {
            return children.at(row);
        }
        return nullptr;
    }
};

class BaseFSModelLoader: public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("BaseFSModelLoader is abstract and cannot be created directly.")

public:
    explicit BaseFSModelLoader(QObject *parent = nullptr);
    virtual ~BaseFSModelLoader() = default;

public slots:
    virtual void loadChildren(FSTreeItem* parentItem) = 0;
    virtual void deleteItem(FSTreeItem* item) = 0;
    virtual void initialize(const QString &path) = 0;
    virtual void setFilter(const QString &filter) = 0;

signals:
    // void itemDeleted(FSTreeItem* item);
    // void childrenLoaded(FSTreeItem* item);
    void filterFinished();
};


#endif //FSCLEANER_BASEFSMODELLOADER_H
