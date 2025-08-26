//
// Created by Dmytrii Zavalnyi on 13.08.2025..
//

#ifndef FSCLEANER_MEDIAINFOEXTRACTOR_H
#define FSCLEANER_MEDIAINFOEXTRACTOR_H
#include <QThread>

#include "mediafileinfo.h"


class MediaInfoExtractor: public QThread {
    Q_OBJECT
public:
    explicit MediaInfoExtractor(const QString &fileName, QObject *parent = nullptr);

protected:
    void run() override;

private:
    QString m_fileName;

signals:
    void resultReady(const MediaFileInfo::FileInfo &ainfo);
};


#endif //FSCLEANER_MEDIAINFOEXTRACTOR_H