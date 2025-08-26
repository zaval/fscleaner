//
// Created by Dmytrii Zavalnyi on 10.08.2025..
//

#ifndef FSCLEANER_FILETYPEICON_H
#define FSCLEANER_FILETYPEICON_H
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>
#include <QMap>

class FileTypeIcon: public QObject{
    Q_OBJECT

    QML_ELEMENT
private:
    QMap<QString, QString> m_icons = {
        {"", "file"},

        {"md", "file-text"},
        {"txt", "file-text"},
        {"html", "file-text"},
        {"htm", "file-text"},
        {"css", "file-text"},
        {"js", "file-text"},
        {"json", "file-text"},
        {"yml", "file-text"},
        {"yaml", "file-text"},
        {"toml", "file-text"},

        {"zip", "file-archive"},
        {"gz", "file-archive"},
        {"rar", "file-archive"},
        {"iso", "file-archive"},
        {"bz", "file-archive"},
        {"gzip", "file-archive"},

        {"jpg", "file-image"},
        {"jpeg", "file-image"},
        {"png", "file-image"},
        {"gif", "file-image"},
        {"tiff", "file-image"},
        {"svg", "file-image"},
        {"bmp", "file-image"},

        {"mp3", "file-music"},
        {"wav", "file-music"},
        {"flac", "file-music"},
        {"acc", "file-music"},
        {"wma", "file-music"},
        {"m4a", "file-music"},
        {"m3u", "file-music"},
        {"m3u8", "file-music"},
        {"pls", "file-music"},

        {"mp4", "file-video"},
        {"avi", "file-video"},
        {"mov", "file-video"},
        {"xvid", "file-video"},
    };

public:
    Q_INVOKABLE QString getIcon(const QString &filePath) const;
};




#endif //FSCLEANER_FILETYPEICON_H
