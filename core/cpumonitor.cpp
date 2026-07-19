#include "cpumonitor.h"

#if defined(Q_OS_LINUX)
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QThread>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#include <QThread>
#endif

#ifdef Q_OS_MAC
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif



CpuMonitor::CpuMonitor(QObject *parent): QObject(parent), m_regexpSeparator{"\\s+"} {}

void CpuMonitor::start() {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CpuMonitor::updateUsage);
    m_timer->start(1000);
}

CpuMonitor::~CpuMonitor() {
    m_timer->stop();
    m_timer->deleteLater();
}

void CpuMonitor::updateUsage() {
    double cpuUsage = 0.0;
    double memUsage = 0.0;
#if defined(Q_OS_LINUX)
    // Note: For Linux, avoid blocking the main thread with msleep.
    // Instead, cache the previous tick values inside your class variables.
    cpuUsage = calculateLinuxCpuUsage();
    memUsage = calculateLinuxMemoryUsage();
#elif defined(Q_OS_WIN)
    cpuUsage = calculateWindowsCpuUsage();
    memUsage = calculateWindowsMemoryUsage();
#elif defined(Q_OS_MAC)
    cpuUsage = calculateMacOsCpuUsage();
    memUsage = calculateMacOsMemoryUsage();
#endif
    emit cpuUsageUpdated(cpuUsage);
    emit memoryUsageUpdated(memUsage);
}

#ifdef Q_OS_MAC
double CpuMonitor::calculateMacOsCpuUsage() {
    // Statics to retain ticks between timer cycles
    static processor_info_array_t prevCpuInfo = nullptr;
    static mach_msg_type_number_t prevNumCpuInfo = 0;
    static natural_t prevNumCpus = 0;

    natural_t numCpus = 0;
    processor_info_array_t cpuInfo = nullptr;
    mach_msg_type_number_t numCpuInfo = 0;

    // Fetch ticks for all individual cores via the Mach kernel
    kern_return_t err = host_processor_info(mach_host_self(),
                                            PROCESSOR_CPU_LOAD_INFO,
                                            &numCpus,
                                            &cpuInfo,
                                            &numCpuInfo);
    if (err != KERN_SUCCESS) {
        return 0.0;
    }

    double totalUsagePercentage = 0.0;

    // If we have history from the previous cycle, compute the delta
    if (prevCpuInfo && numCpus == prevNumCpus) {
        for (unsigned int i = 0; i < numCpus; ++i) {
            // Indexing matching the CPU_STATE blocks per processor core
            unsigned int baseIdx = CPU_STATE_MAX * i;

            uint32_t userDiff   = cpuInfo[baseIdx + CPU_STATE_USER]   - prevCpuInfo[baseIdx + CPU_STATE_USER];
            uint32_t systemDiff = cpuInfo[baseIdx + CPU_STATE_SYSTEM] - prevCpuInfo[baseIdx + CPU_STATE_SYSTEM];
            uint32_t niceDiff   = cpuInfo[baseIdx + CPU_STATE_NICE]   - prevCpuInfo[baseIdx + CPU_STATE_NICE];
            uint32_t idleDiff   = cpuInfo[baseIdx + CPU_STATE_IDLE]   - prevCpuInfo[baseIdx + CPU_STATE_IDLE];

            uint32_t activeTicks = userDiff + systemDiff + niceDiff;
            uint32_t totalTicks  = activeTicks + idleDiff;

            if (totalTicks > 0) {
                totalUsagePercentage += (double)activeTicks / totalTicks;
            }
        }
        // Normalize against total core count to get system average
        totalUsagePercentage = (totalUsagePercentage / numCpus) * 100.0;
    }

    // Clean up previous historical memory allocations from the Mach layer
    if (prevCpuInfo) {
        vm_deallocate(mach_task_self(), (vm_address_t)prevCpuInfo, sizeof(integer_t) * prevNumCpuInfo);
    }

    // Cache current pointers to become history for the next interval pass
    prevCpuInfo = cpuInfo;
    prevNumCpuInfo = numCpuInfo;
    prevNumCpus = numCpus;

    return totalUsagePercentage;
}

double CpuMonitor::calculateMacOsMemoryUsage() {
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmStats;

    kern_return_t kr = host_statistics64(mach_host_self(),
                                         HOST_VM_INFO64,
                                         reinterpret_cast<host_info64_t>(&vmStats),
                                         &count);
    if (kr != KERN_SUCCESS) return 0.0;

    // Get system page size (typically 4096 or 16384 bytes depending on Intel/Apple Silicon)
    vm_size_t pageSize;
    host_page_size(mach_host_self(), &pageSize);

    // Activity Monitor formula for "Memory Used"
    uint64_t activePages     = vmStats.active_count;
    uint64_t wiredPages      = vmStats.wire_count;
    uint64_t compressedPages = vmStats.compressor_page_count;
    uint64_t purgeablePages  = vmStats.purgeable_count;
    uint64_t externalPages   = vmStats.external_page_count; // file-backed memory

    // Total used memory tracking
    uint64_t usedMemory = (activePages + wiredPages + compressedPages + purgeablePages + externalPages) * pageSize;

    // Total physical RAM installed
    int64_t totalRam = 0;
    size_t len = sizeof(totalRam);
    sysctlbyname("hw.memsize", &totalRam, &len, nullptr, 0);

    if (totalRam == 0) return 0.0;
    return (double)usedMemory / totalRam * 100.0;
}
#endif


#ifdef Q_OS_WIN
double CpuMonitor::calculateWindowsMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo)) {
        return static_cast<double>(memInfo.dwMemoryLoad); // Directly returns the % of physical memory in use
    }
    return 0.0;
}
double CpuMonitor::calculateWindowsCpuUsage() {
    static FILETIME idleTime, kernelTime, userTime;
    static bool firstRun = true;

    if (firstRun) {
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        QThread::msleep(500);
        firstRun = false;
    }

    FILETIME prevIdleTime = idleTime;
    FILETIME prevKernelTime = kernelTime;
    FILETIME prevUserTime = userTime;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0.0;

    auto convert = [](const FILETIME& ft) {
        return (uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    uint64_t idle = convert(idleTime) - convert(prevIdleTime);
    uint64_t kernel = convert(kernelTime) - convert(prevKernelTime);
    uint64_t user = convert(userTime) - convert(prevUserTime);
    uint64_t system = kernel + user;

    if (system == 0) return 0.0;
    return static_cast<double>(system - idle) / system * 100.0;
}
#endif


#if defined(Q_OS_LINUX)
double CpuMonitor::calculateLinuxMemoryUsage() {
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 0.0;

    QTextStream in(&file);
    uint64_t totalMem = 0;
    uint64_t availMem = 0;

    QString line;
    while (in.readLineInto(&line)) {
        if (line.startsWith("MemTotal:")) {
            totalMem = line.split(m_regexpSeparator).at(1).toULongLong();
        } else if (line.startsWith("MemAvailable:")) {
            availMem = line.split(m_regexpSeparator).at(1).toULongLong();
            break; // Found what we need
        }
    }

    if (totalMem == 0) return 0.0;
    const uint64_t usedMem = totalMem - availMem;
    return static_cast<double>(usedMem) / static_cast<double>(totalMem) * 100.0;
}
CpuTicks CpuMonitor::getLinuxCpuTicks() {
    QFile file("/proc/stat");
    CpuTicks ticks = {0, 0, 0, 0};
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        const QString line = in.readLine(); // Reads the first line starting with "cpu "
        QStringList values = line.split(m_regexpSeparator, Qt::SkipEmptyParts);
        if (values.size() >= 5) {
            ticks.user = values[1].toULongLong();
            ticks.nice = values[2].toULongLong();
            ticks.system = values[3].toULongLong();
            ticks.idle = values[4].toULongLong();
        }
    }
    return ticks;
}

double CpuMonitor::calculateLinuxCpuUsage() {
    const CpuTicks t1 = getLinuxCpuTicks();
    QThread::msleep(500); // Wait 500ms for a valid delta sample
    const CpuTicks t2 = getLinuxCpuTicks();

    const unsigned long long active1 = t1.user + t1.nice + t1.system;
    const unsigned long long total1 = active1 + t1.idle;

    const unsigned long long active2 = t2.user + t2.nice + t2.system;
    const unsigned long long total2 = active2 + t2.idle;

    if (total2 == total1) return 0.0;
    return static_cast<double>(active2 - active1) / static_cast<double>(total2 - total1) * 100.0;
}
#endif

