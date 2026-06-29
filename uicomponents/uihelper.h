#ifndef FSCLEANER_UIHELPER_H
#define FSCLEANER_UIHELPER_H
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

class UIHelper: public QObject{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString homeDirectory READ homeDirectory NOTIFY homeDirectoryChanged)
    Q_PROPERTY(QString documentsDirectory READ documentsDirectory NOTIFY documentsDirectoryChanged)
    Q_PROPERTY(QString downloadsDirectory READ downloadsDirectory NOTIFY downloadsDirectoryChanged)
    Q_PROPERTY(QString cacheDirectory READ cacheDirectory NOTIFY cacheDirectoryChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)

public:
    QString homeDirectory() const;
    QString documentsDirectory() const;
    QString downloadsDirectory() const;
    QString cacheDirectory() const;
    QString version() const;

    Q_INVOKABLE QString getFolderName(const QString &path) const;
    Q_INVOKABLE QString getFileName(const QString &path) const;
    Q_INVOKABLE bool remove(const QString &path);

signals:
    void homeDirectoryChanged();
    void documentsDirectoryChanged();
    void downloadsDirectoryChanged();
    void cacheDirectoryChanged();
    void versionChanged();


};


#endif //FSCLEANER_UIHELPER_H
