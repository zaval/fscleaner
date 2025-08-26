#ifndef HASHCALCULATOR_H
#define HASHCALCULATOR_H

#include <QObject>
#include <QThread>
#include <QCryptographicHash>


class HashCalculator : public QThread
{
    Q_OBJECT
public:
    explicit HashCalculator(const QString &filePath, const QCryptographicHash::Algorithm method, QObject *parent = nullptr);

protected:
    void run() override;

private:
    QString m_filePath;
    QCryptographicHash::Algorithm m_method;
signals:
    void resultReady(const QString &s);
};


#endif // HASHCALCULATOR_H
