#ifndef FSCLEANER_CPUMONITOR_H
#define FSCLEANER_CPUMONITOR_H
#include <QObject>
#include <QRegularExpression>
#include <QTimer>


#if defined(Q_OS_LINUX)
struct CpuTicks {
    unsigned long long user, nice, system, idle;
};
#endif


class CpuMonitor: public QObject {
    Q_OBJECT

public:
    explicit CpuMonitor(QObject *parent = nullptr);
    ~CpuMonitor();

public slots:
    void start();

signals:
    void cpuUsageUpdated(double percentage);
    void memoryUsageUpdated(double percentage);

private slots:
    void updateUsage();

private:
    QTimer *m_timer;
    QRegularExpression m_regexpSeparator;

#if defined(Q_OS_LINUX)
    CpuTicks getLinuxCpuTicks();
    double calculateLinuxCpuUsage();
    double calculateLinuxMemoryUsage();
#endif

#ifdef Q_OS_WIN
    double calculateWindowsCpuUsage();
    double calculateWindowsMemoryUsage();
#endif

#ifdef Q_OS_MACOS
    double calculateMacOsCpuUsage();
    double calculateMacOsMemoryUsage();
#endif


};


#endif //FSCLEANER_CPUMONITOR_H
