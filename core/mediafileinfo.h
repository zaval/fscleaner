//
// Created by Dmytrii Zavalnyi on 13.08.2025..
//

#ifndef FSCLEANER_MEDIAFILEINFO_H
#define FSCLEANER_MEDIAFILEINFO_H
#include <QMap>
#include <QString>




class MediaFileInfo {
public:
    MediaFileInfo();

    struct AudioInfo {
        QString filename;
        double duration = 0;
        int sample_rate = 0;
        int channels = 0;
        int64_t bit_rate = 0;
        QString codec_name = {};
        QString format_name = {};
        QMap<QString, QString> metadata = {};
        bool is_valid = false;

    };

    struct VideoInfo {
        int width = 0;
        int height = 0;
        double duration = 0;
        int64_t bit_rate = 0;
        QString codec_name = {};
        QString format_name = {};
        QMap<QString, QString> metadata = {};
        bool is_valid = false;
    };

    struct FileInfo {
        QString filename;
        AudioInfo audio;
        VideoInfo video;
        bool is_valid = false;
    };

    static FileInfo getMediaInfo(const QString &filename);
    static QString getDurationString(double duration);



};


#endif //FSCLEANER_MEDIAFILEINFO_H