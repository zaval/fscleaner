#ifndef HELPER_H
#define HELPER_H
#include <QString>

class Helper
{
public:
    Helper();

    static QString formatSize(qint64 bytes);
};

#endif // HELPER_H
