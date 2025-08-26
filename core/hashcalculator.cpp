#include "hashcalculator.h"


#include <QFile>
#include <QIODevice>

HashCalculator::HashCalculator(const QString &filePath, const QCryptographicHash::Algorithm method, QObject *parent) : QThread(parent),
    m_filePath(filePath),
    m_method(method) {}

void HashCalculator::run()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit resultReady("Error opening file");
        return;
    }

    QCryptographicHash hash(m_method);
    if (!hash.addData(&file)) {
        emit resultReady("Error reading file");
        return;
    }

    QString md5Hash = hash.result().toHex();
    emit resultReady(md5Hash);
}


