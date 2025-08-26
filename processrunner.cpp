#include "processrunner.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>


ProcessRunner::ProcessRunner(QObject *parent)
    : QObject(parent),
    m_logMessageLine(""),
    m_isRunning(false)
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataPath);
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

    connect(m_process, &QProcess::finished, this, [&](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "Process finished with exit code:" << exitCode << "and status:" << exitStatus;
        emit processFinished();
        m_isRunning = false;
        emit isRunningChanged();
    });

    connect(m_process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error){
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
    const auto fsScan = QCoreApplication::applicationDirPath() + "/fsscan";
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
    m_process->start(command, args);
    m_isRunning = true;
    emit isRunningChanged();
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
