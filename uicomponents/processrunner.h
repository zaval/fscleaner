#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QThread>

// #include <QtQmlIntegration>
#include <QtQmlIntegration/qqmlintegration.h>

#include "../core/cpumonitor.h"

class ProcessRunner : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString logMessageLine READ logMessageLine NOTIFY logMessageLineChanged FINAL)
    Q_PROPERTY(QString dbPath READ dbPath NOTIFY dbPathChanged FINAL)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged FINAL)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(double memoryUsage READ memoryUsage NOTIFY memoryUsageChanged)
    Q_PROPERTY(QString timeElapsed READ timeElapsed NOTIFY timeElapsedChanged)
    Q_PROPERTY(int fps READ fps NOTIFY fpsChanged)

public:
    explicit ProcessRunner(QObject *parent = nullptr);
    ~ProcessRunner();

    static ProcessRunner *create(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE void processFolder(const QString &folder, bool ignoreMounts = false);
    Q_INVOKABLE void stopProcess();

    QString logMessageLine() const;

    QString dbPath() const;

    bool isRunning() const;

    double cpuUsage() const;

    double memoryUsage() const;
    QString timeElapsed() const;
    int fps() const;



signals:
    void logMessageLineChanged();
    void processFinished();
    void dbPathChanged();
    void cpuUsageChanged();

    void isRunningChanged();
    void memoryUsageChanged();
    void timeElapsedChanged();
    void fpsChanged();

private:
    QString m_logMessageLine;

    QProcess *m_process;

    void runProcess(const QString &command, const QStringList &args);
    QString m_dbPath;
    bool m_isRunning;
    double m_cpuUsage;

    QThread m_cpuThread;
    CpuMonitor *m_cpuMonitor;
    double m_memoryUsage;
    QString m_timeElapsed;
    quint64 m_workerTime;
    QTimer *m_workerTimer;

    QString timeToHMS(quint64 tm) const;
    int m_fps;
};

#endif // PROCESSRUNNER_H
