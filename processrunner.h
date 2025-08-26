#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QProcess>
#include <QQmlEngine>

// #include <QtQmlIntegration>
#include <QtQmlIntegration/qqmlintegration.h>

class ProcessRunner : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString logMessageLine READ logMessageLine NOTIFY logMessageLineChanged FINAL)
    Q_PROPERTY(QString dbPath READ dbPath NOTIFY dbPathChanged FINAL)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged FINAL)


public:
    explicit ProcessRunner(QObject *parent = nullptr);

    static ProcessRunner *create(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE void processFolder(const QString &folder, bool ignoreMounts = false);
    Q_INVOKABLE void stopProcess();

    QString logMessageLine() const;

    QString dbPath() const;

    bool isRunning() const;

signals:
    void logMessageLineChanged();
    void processFinished();
    void dbPathChanged();

    void isRunningChanged();

private:
    QString m_logMessageLine;

    QProcess *m_process;

    void runProcess(const QString &command, const QStringList &args);
    QString m_dbPath;
    bool m_isRunning;
};

#endif // PROCESSRUNNER_H
