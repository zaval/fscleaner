#ifndef FSCLEANER_FILESELECTOR_H
#define FSCLEANER_FILESELECTOR_H
#include <QObject>
#include <qqmlintegration.h>


class FileSelector: public QObject {
    Q_OBJECT

    Q_PROPERTY(QString caption READ caption WRITE setCaption NOTIFY captionChanged)
    Q_PROPERTY(QString folder READ folder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)

    QML_ELEMENT

public:
    explicit FileSelector(QObject *parent = nullptr);
    QString caption() const;
    void setCaption(const QString &newCaption);
    QString folder() const;
    void setFolder(const QString &newFolder);
    QString filter() const;
    void setFilter(const QString &newFilter);

    Q_INVOKABLE void showFolderSelector();
    // Q_INVOKABLE void showFileSelector();
    // Q_INVOKABLE void showNewFileSelector();
signals:
    void captionChanged();
    void folderChanged();
    void filterChanged();
    void accepted(QString path);
private:
    QString m_filter;
    QString m_caption;
    QString m_folder;
};


#endif //FSCLEANER_FILESELECTOR_H
