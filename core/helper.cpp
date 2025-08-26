#include "helper.h"
#include <QLocale>

Helper::Helper() {}

QString Helper::formatSize(qint64 bytes)
{
    if (bytes < 0) return {}; // Or handle error

    constexpr qint64 kilobyte = 1024;
    constexpr qint64 megabyte = kilobyte * 1024;
    constexpr qint64 gigabyte = megabyte * 1024;
    constexpr qint64 terabyte = gigabyte * 1024;

    const auto locale = QLocale::system(); // Use system locale for number formatting

    if (bytes >= terabyte)
        return locale.toString(static_cast<double>(bytes) / terabyte, 'f', 2) + " TB";
    if (bytes >= gigabyte)
        return locale.toString(static_cast<double>(bytes) / gigabyte, 'f', 2) + " GB";
    if (bytes >= megabyte)
        return locale.toString(static_cast<double>(bytes) / megabyte, 'f', 2) + " MB";
    if (bytes >= kilobyte)
        return locale.toString(static_cast<double>(bytes) / kilobyte, 'f', 1) + " KB";

    return locale.toString(bytes) + " Bytes";
}
