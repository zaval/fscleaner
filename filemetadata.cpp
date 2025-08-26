#include "filemetadata.h"

#include "core/hashcalculator.h"
#include "core/helper.h"

#include <QMimeDatabase>
#include <QIcon>
#include <QFileInfo>
#include <QThread>

#include "core/mediainfoextractor.h"

const QStringList FileMetaData::s_archive_mimetypes = {
    "application/zip",
    "application/x-bzip",
    "application/java-archive",
    "application/gzip",
    "application/vnd.android.package-archive",
};

int FileMetaData::imageWidth() const {
    return m_imageWidth;
}

void FileMetaData::setImageWidth(const int &newImageWidth) {
    if (m_imageWidth == newImageWidth)
        return;
    m_imageWidth = newImageWidth;
    emit imageWidthChanged();
}

int FileMetaData::imageHeight() const {
    return m_imageHeight;
}

void FileMetaData::setImageHeight(const int &newImageHeight) {
    if (m_imageHeight == newImageHeight)
        return;
    m_imageHeight = newImageHeight;
    emit imageHeightChanged();
}

int FileMetaData::depth() const {
    return m_depth;
}

void FileMetaData::setDepth(const int &newDepth) {
    if (m_depth == newDepth)
        return;
    m_depth = newDepth;
    emit depthChanged();

}

QString FileMetaData::audioDuration() const {
    return m_audioDuration;
}

void FileMetaData::setAudioDuration(const QString &newAudioDuration) {
    if (m_audioDuration == newAudioDuration)
        return;
    m_audioDuration = newAudioDuration;
    emit audioDurationChanged();

}

int FileMetaData::audioSampleRate() const {
    return m_audioSampleRate;
}

void FileMetaData::setAudioSampleRate(const int &newAudioSampleRate) {
    if (m_audioSampleRate == newAudioSampleRate)
        return;
    m_audioSampleRate = newAudioSampleRate;
    emit audioSampleRateChanged();

}

int FileMetaData::audioChannels() const {
    return m_audioChannels;
}

void FileMetaData::setAudioChannels(const int &newAudioChannels) {
    if (m_audioChannels == newAudioChannels)
        return;
    m_audioChannels = newAudioChannels;
    emit audioChannelsChanged();

}

int64_t FileMetaData::audioBitRate() const {
    return m_audioBitRate;
}

void FileMetaData::setAudioBitRate(const int64_t &newAudioBitRate) {
    if (m_audioBitRate == newAudioBitRate)
        return;
    m_audioBitRate = newAudioBitRate;
    emit audioBitRateChanged();

}

QString FileMetaData::audioCodecName() const {
    return m_audioCodecName;
}

void FileMetaData::setAudioCodecName(const QString &newAudioCodecName) {
    if (m_audioCodecName == newAudioCodecName)
        return;
    m_audioCodecName = newAudioCodecName;
    emit audioCodecNameChanged();

}

QString FileMetaData::audioFormatName() const {
    return m_audioFormatName;
}

void FileMetaData::setAudioFormatName(const QString &newAudioFormatName) {
    if (m_audioFormatName == newAudioFormatName)
        return;
    m_audioFormatName = newAudioFormatName;
    emit audioFormatNameChanged();

}

QString FileMetaData::videoFormatName() const {
    return m_videoFormatName;
}

void FileMetaData::setVideoFormatName(const QString &newVideoFormatName) {
    if (m_videoFormatName == newVideoFormatName)
        return;
    m_videoFormatName = newVideoFormatName;
    emit videoFormatNameChanged();

}

QString FileMetaData::videoCodecName() const {
    return m_videoCodecName;
}

void FileMetaData::setVideoCodecName(const QString &newVideoCodecName) {
    if (m_videoCodecName == newVideoCodecName)
        return;
    m_videoCodecName = newVideoCodecName;
    emit videoCodecNameChanged();

}

int64_t FileMetaData::videoBitRate() const {
    return m_videoBitRate;
}

void FileMetaData::setVideoBitRate(const int64_t &newVideoBitRate) {
    if (m_videoBitRate == newVideoBitRate)
        return;
    m_videoBitRate = newVideoBitRate;
    emit videoBitRateChanged();

}

int FileMetaData::videoSampleRate() const {
    return m_videoSampleRate;
}

void FileMetaData::setVideoSampleRate(const int &newVideoSampleRate) {
    if (m_videoSampleRate == newVideoSampleRate)
        return;
    m_videoSampleRate = newVideoSampleRate;
    emit videoSampleRateChanged();

}

QString FileMetaData::videoDuration() const {
    return m_videoDuration;
}

void FileMetaData::setVideoDuration(const QString &newVideoDuration) {
    if (m_videoDuration == newVideoDuration)
        return;
    m_videoDuration = newVideoDuration;
    emit videoDurationChanged();

}

bool FileMetaData::isVideo() const {
    return m_isVideo;
}

void FileMetaData::setIsVideo(const bool &newIsVideo) {
    if (m_isVideo == newIsVideo)
        return;
    m_isVideo = newIsVideo;
    emit isVideoChanged();

}

int FileMetaData::videoHeight() const {
    return m_videoHeight;
}

void FileMetaData::setVideoHeight(const int &newVideoHeight) {
    if (m_videoHeight == newVideoHeight)
        return;
    m_videoHeight = newVideoHeight;
    emit videoHeightChanged();

}

int FileMetaData::videoWidth() const {
    return m_videoWidth;
}

void FileMetaData::setVideoWidth(const int &newVideoWidth) {
    if (m_videoWidth == newVideoWidth)
        return;
    m_videoWidth = newVideoWidth;
    emit videoWidthChanged();

}

bool FileMetaData::isArchive() const {
    return m_isArchive;
}

void FileMetaData::setIsArchive(const bool &newIsArchive) {
    if (m_isArchive == newIsArchive)
        return;
    m_isArchive = newIsArchive;
    emit isArchiveChanged();

}

FileMetaData::FileMetaData(QObject *parent)
    : QObject{parent},
    m_path(""),
    m_fileName(""),
    m_fileSize(0),
    m_isPlainText(false),
    m_isImage(false),
    m_icon(""),
    m_textContent(""),
    m_formattedSize(""),
    m_md5Hash("--"),
    m_sha256Hash("--"),
    m_createdTime(""),
    m_modifiedTime(""),
    m_user(""),
    m_group(""),
    m_imageWidth(0),
    m_imageHeight(0),
    m_depth(0),
    m_isAudio(false),
    m_audioSampleRate(0),
    m_audioChannels(0),
    m_audioBitRate(0)
{}

FileMetaData *FileMetaData::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new FileMetaData();
}

QString FileMetaData::path() const
{
    return m_path;
}

void FileMetaData::setPath(const QString &newPath)
{

    if (m_path == newPath)
        return;

    if (newPath.isEmpty()){
        m_path = newPath;
        setFileSize(0);
        setFileName("");
        setIsPlainText(false);
        setIsImage(false);
        setIcon("");
        setTextContent("");
        setFormattedSize("");
        setMd5Hash("--");
        setSha256Hash("--");
        setCreatedTime("");
        setModifiedTime("");
        setUser("");
        setGroup("");
        setImageWidth(0);
        setImageHeight(0);
        setDepth(0);
        setIsAudio(false);
        emit pathChanged();
        return;
    }

    setIsAudio(false);
    setIsImage(false);
    setIsPlainText(false);
    setIsVideo(false);
    setIsArchive(false);

    m_path = newPath;
    emit pathChanged();
    setMd5Hash("--");
    setSha256Hash("--");

    auto *md5 = new HashCalculator(newPath, QCryptographicHash::Md5, nullptr);
    connect(md5, &HashCalculator::resultReady, this, [&](const QString &result){
        setMd5Hash(result);
    });
    connect(md5, &HashCalculator::finished, md5, &QObject::deleteLater);
    md5->start();

    const auto fi = QFileInfo(newPath);
    setFileName(fi.fileName());
    setFileSize(fi.size());
    setFormattedSize(Helper::formatSize(fi.size()));
    setCreatedTime(fi.birthTime().toString(Qt::ISODate));
    setModifiedTime(fi.lastModified().toString(Qt::ISODate));
    setUser(fi.owner());
    setGroup(fi.group());
    setIsImage(false);
    setIsPlainText(false);

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(newPath);

    qDebug() << "MIME" << mime.name();

    // setIcon(mime.iconName());
    setIcon("file");

    if (mime.inherits("text/plain")){
        setIsPlainText(true);
        setIcon("file-text");

        if (fi.size() < 1024 * 1024 * 2){
            QFile file(newPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                setTextContent(file.readAll());
            }
            else{
                setTextContent("");
            }

        }

    }
    if (mime.name().startsWith("image/")){
        setIcon("file-image");
        const auto image = QImage(newPath);
        if (image.width() > 0 && image.height() > 0) {
            setIsImage(true);
            setImageWidth(image.width());
            setImageHeight(image.height());
            setDepth(image.depth());
        }

    }

    if (mime.name().startsWith("audio/") || mime.name().startsWith("video/")) {
        auto *mediaExtractor = new MediaInfoExtractor(newPath, nullptr);
        connect(mediaExtractor, &MediaInfoExtractor::finished, mediaExtractor, &QObject::deleteLater);
        connect(mediaExtractor, &MediaInfoExtractor::resultReady, this, [&](const MediaFileInfo::FileInfo &info) {
            if (info.audio.is_valid) {
                setIcon("file-music");
                setAudioBitRate(info.audio.bit_rate);
                setAudioChannels(info.audio.channels);
                setAudioSampleRate(info.audio.sample_rate);
                setAudioCodecName(info.audio.codec_name);
                setAudioFormatName(info.audio.format_name);
                setAudioDuration(MediaFileInfo::getDurationString(info.audio.duration));
                setIsAudio(true);
            }

            if (info.video.is_valid) {
                setIcon("file-video");
                setVideoBitRate(info.video.bit_rate);
                setVideoCodecName(info.video.codec_name);
                setVideoFormatName(info.video.format_name);
                setVideoDuration(MediaFileInfo::getDurationString(info.video.duration));
                setVideoHeight(info.video.height);
                setVideoWidth(info.video.width);
                setIsVideo(true);
            }


        });
        mediaExtractor->start();
    }

    if (s_archive_mimetypes.contains(mime.name())){
        setIsArchive(true);
        setIcon("file-archive");
    }
}

QString FileMetaData::fileName() const
{
    return m_fileName;
}

void FileMetaData::setFileName(const QString &newFileName)
{
    if (m_fileName == newFileName)
        return;
    m_fileName = newFileName;
    emit fileNameChanged();
}

qint64 FileMetaData::fileSize() const
{
    return m_fileSize;
}

void FileMetaData::setFileSize(qint64 newFileSize)
{
    if (m_fileSize == newFileSize)
        return;
    m_fileSize = newFileSize;
    emit fileSizeChanged();
}

bool FileMetaData::isPlainText() const
{
    return m_isPlainText;
}

void FileMetaData::setIsPlainText(bool newIsPlainText)
{
    if (m_isPlainText == newIsPlainText)
        return;
    m_isPlainText = newIsPlainText;
    emit isPlainTextChanged();
}

bool FileMetaData::isImage() const
{
    return m_isImage;
}

void FileMetaData::setIsImage(bool newIsImage)
{
    if (m_isImage == newIsImage)
        return;
    m_isImage = newIsImage;
    emit isImageChanged();
}

bool FileMetaData::isAudio() const {
    return m_isAudio;
}

void FileMetaData::setIsAudio(const bool &newIsAudio) {
    if (m_isAudio == newIsAudio)
        return;
    m_isAudio = newIsAudio;
    emit isAudioChanged();
}

QString FileMetaData::icon() const
{
    return m_icon;
}

void FileMetaData::setIcon(const QString &newIcon)
{
    if (m_icon == newIcon)
        return;
    m_icon = newIcon;
    emit iconChanged();
}

QString FileMetaData::textContent() const
{
    return m_textContent;
}

void FileMetaData::setTextContent(const QString &newTextContent)
{
    if (m_textContent == newTextContent)
        return;
    m_textContent = newTextContent;
    emit textContentChanged();
}

QString FileMetaData::formattedSize() const
{
    return m_formattedSize;
}

void FileMetaData::setFormattedSize(const QString &newFormattedSize)
{
    if (m_formattedSize == newFormattedSize)
        return;
    m_formattedSize = newFormattedSize;
    emit formattedSizeChanged();
}

QString FileMetaData::md5Hash() const
{
    return m_md5Hash;
}

void FileMetaData::setMd5Hash(const QString &newMd5Hash)
{
    if (m_md5Hash == newMd5Hash)
        return;
    m_md5Hash = newMd5Hash;
    emit md5HashChanged();
}

QString FileMetaData::sha256Hash() const
{
    return m_sha256Hash;
}

void FileMetaData::setSha256Hash(const QString &newSha256Hash)
{
    if (m_sha256Hash == newSha256Hash)
        return;
    m_sha256Hash = newSha256Hash;
    emit sha256HashChanged();
}

QString FileMetaData::createdTime() const
{
    return m_createdTime;
}

void FileMetaData::setCreatedTime(const QString &newCreatedTime)
{
    if (m_createdTime == newCreatedTime)
        return;
    m_createdTime = newCreatedTime;
    emit createdTimeChanged();
}

QString FileMetaData::modifiedTime() const
{
    return m_modifiedTime;
}

void FileMetaData::setModifiedTime(const QString &newModifiedTime)
{
    if (m_modifiedTime == newModifiedTime)
        return;
    m_modifiedTime = newModifiedTime;
    emit modifiedTimeChanged();
}

QString FileMetaData::user() const
{
    return m_user;
}

void FileMetaData::setUser(const QString &newUser)
{
    if (m_user == newUser)
        return;
    m_user = newUser;
    emit userChanged();
}

QString FileMetaData::group() const
{
    return m_group;
}

void FileMetaData::setGroup(const QString &newGroup)
{
    if (m_group == newGroup)
        return;
    m_group = newGroup;
    emit groupChanged();
}
