#include "processrunner.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>


ProcessRunner::ProcessRunner(QObject *parent)
    : QObject(parent),
    m_logMessageLine(""),
    m_isRunning(false),
    m_timeElapsed("00:00:00"),
    m_workerTime(0),
    m_fps(0)
{
    const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        dataDir.mkpath("."); // Create the directory if it doesn't exist
    }
    m_dbPath = dataPath + "/filesystem.db";
    emit dbPathChanged();

    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, [&](){
        const auto data = m_process->readAllStandardOutput();
        // qDebug() << "Output:" << data;
        const auto messages = QString::fromUtf8(data).split("\n");
        const auto sz = messages.size();

        m_logMessageLine = sz > 1 ? messages[sz-2] : messages.last();
        emit logMessageLineChanged();
    });

    connect(m_process, &QProcess::finished, this, [&](const int exitCode, const QProcess::ExitStatus exitStatus) {
        qDebug() << "Process finished with exit code:" << exitCode << "and status:" << exitStatus;
        emit processFinished();
        m_isRunning = false;
        emit isRunningChanged();
        if (m_workerTimer) {
            m_workerTimer->stop();
            m_workerTimer->deleteLater();
            m_workerTimer = nullptr;
        }
        // m_timeElapsed = "00:00:00";
        // emit timeElapsedChanged();
    });

    connect(m_process, &QProcess::errorOccurred, this, [&](const QProcess::ProcessError error){
        m_isRunning = false;
        emit isRunningChanged();
        qDebug() << "Process error:" << error;
        auto message = "";
        switch (error) {
        case QProcess::FailedToStart:
            message = "The process failed to start.";
            break;
        case QProcess::Crashed:
            message = "The process crashed.";
            break;
        case QProcess::Timedout:
            message = "The process timed out.";
            break;
        case QProcess::WriteError:
            message = "The process encountered a write error.";
            break;
        case QProcess::ReadError:
            message = "The process encountered a read error.";
            break;
        case QProcess::UnknownError:
            message = "An unknown error occurred.";
            break;
        default:
            break;
        }
        m_logMessageLine = QString(message);
        emit logMessageLineChanged();
    });

    m_cpuMonitor = new CpuMonitor();
    m_cpuMonitor->moveToThread(&m_cpuThread);

    connect(&m_cpuThread, &QThread::started, m_cpuMonitor, &CpuMonitor::start);
    connect(&m_cpuThread, &QThread::finished, m_cpuMonitor, &CpuMonitor::deleteLater);

    connect(m_cpuMonitor, &CpuMonitor::cpuUsageUpdated, this, [&](double percentage){
        m_cpuUsage = percentage;
        emit cpuUsageChanged();
    });

    connect(m_cpuMonitor, &CpuMonitor::memoryUsageUpdated, this, [&](double percentage){
        m_memoryUsage = percentage;
        emit memoryUsageChanged();
    });

    m_cpuThread.start();
}

ProcessRunner::~ProcessRunner() {
    m_cpuThread.quit();
    m_cpuThread.wait();
}

ProcessRunner *ProcessRunner::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new ProcessRunner();
}

void ProcessRunner::processFolder(const QString &folder, bool ignoreMounts)
{

    // const auto folder = current executable file path
#ifdef Q_OS_LINUX
    auto fsScan = QStandardPaths::findExecutable("fsscan");
    if (fsScan.isEmpty())
    {
        fsScan = QCoreApplication::applicationDirPath() + "/fsscan/fsscan";
    }
#else
    const auto fsScan = QCoreApplication::applicationDirPath() + "/fsscan";
#endif

    auto args = QStringList() << "-d" << m_dbPath << folder;
    if (ignoreMounts) {
        args << "-i";
    }

    runProcess(
        fsScan,
        args
        // QStringList()
        //     << "-d"
        //     << m_dbPath
        //     << folder
        );


}

void ProcessRunner::stopProcess()
{
    m_process->kill();

}

void ProcessRunner::runProcess(const QString &command, const QStringList &args)
{
    qDebug() << command << args.join(" ");
    m_workerTime = 0;
    m_process->start(command, args);
    m_isRunning = true;
    emit isRunningChanged();
    m_workerTimer = new QTimer(this);
    connect(m_workerTimer, &QTimer::timeout, this, [&](){
        m_workerTime++;
        m_timeElapsed = timeToHMS(m_workerTime);
        emit timeElapsedChanged();
    });
    m_workerTimer->setSingleShot(false);
    m_workerTimer->setInterval(1000);
    m_workerTimer->start();
}

QString ProcessRunner::timeToHMS(const quint64 tm) const {
    const auto h = tm / 3600;
    const auto m = (tm % 3600) / 60;
    const auto s = tm % 60;
    return QTime(h, m, s).toString("HH:mm:ss");
}

QString ProcessRunner::logMessageLine() const
{
    return m_logMessageLine;
}

QString ProcessRunner::dbPath() const
{
    return m_dbPath;
}

bool ProcessRunner::isRunning() const
{
    return m_isRunning;
}

double ProcessRunner::cpuUsage() const {
    return m_cpuUsage;
}

double ProcessRunner::memoryUsage() const {
    return m_memoryUsage;
}

QString ProcessRunner::timeElapsed() const {
    return m_timeElapsed;
}

int ProcessRunner::fps() const {
    return m_fps;
}
