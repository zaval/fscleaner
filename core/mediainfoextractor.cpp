//
// Created by Dmytrii Zavalnyi on 13.08.2025..
//

#include "mediainfoextractor.h"

MediaInfoExtractor::MediaInfoExtractor(const QString &fileName, QObject *parent):
    QThread(parent),
    m_fileName(fileName)
{}

void MediaInfoExtractor::run() {
    const auto fileInfo = MediaFileInfo::getMediaInfo(m_fileName);
    emit resultReady(fileInfo);
}
