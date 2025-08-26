#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <QObject>
#include <QQmlEngine>

class FileMetaData : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    // QML_SINGLETON

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged FINAL)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(qint64 fileSize READ fileSize WRITE setFileSize NOTIFY fileSizeChanged FINAL)
    Q_PROPERTY(bool isPlainText READ isPlainText WRITE setIsPlainText NOTIFY isPlainTextChanged FINAL)
    Q_PROPERTY(bool isImage READ isImage WRITE setIsImage NOTIFY isImageChanged FINAL)
    Q_PROPERTY(bool isAudio READ isAudio WRITE setIsAudio NOTIFY isAudioChanged FINAL)
    Q_PROPERTY(bool isVideo READ isVideo WRITE setIsVideo NOTIFY isVideoChanged FINAL)
    Q_PROPERTY(bool isArchive READ isArchive WRITE setIsArchive NOTIFY isArchiveChanged FINAL)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged FINAL)
    Q_PROPERTY(QString textContent READ textContent WRITE setTextContent NOTIFY textContentChanged FINAL)
    Q_PROPERTY(QString formattedSize READ formattedSize WRITE setFormattedSize NOTIFY formattedSizeChanged FINAL)
    Q_PROPERTY(QString md5Hash READ md5Hash WRITE setMd5Hash NOTIFY md5HashChanged FINAL)
    Q_PROPERTY(QString sha256Hash READ sha256Hash WRITE setSha256Hash NOTIFY sha256HashChanged FINAL)
    Q_PROPERTY(QString createdTime READ createdTime WRITE setCreatedTime NOTIFY createdTimeChanged FINAL)
    Q_PROPERTY(QString modifiedTime READ modifiedTime WRITE setModifiedTime NOTIFY modifiedTimeChanged FINAL)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged FINAL)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged FINAL)

    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged FINAL)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged FINAL)
    Q_PROPERTY(int depth READ depth WRITE setDepth NOTIFY depthChanged FINAL)

    Q_PROPERTY(QString audioDuration READ audioDuration WRITE setAudioDuration NOTIFY audioDurationChanged)
    Q_PROPERTY(int audioSampleRate READ audioSampleRate WRITE setAudioSampleRate NOTIFY audioSampleRateChanged FINAL)
    Q_PROPERTY(int audioChannels READ audioChannels WRITE setAudioChannels NOTIFY audioChannelsChanged FINAL)
    Q_PROPERTY(int64_t audioBitRate READ audioBitRate WRITE setAudioBitRate NOTIFY audioBitRateChanged FINAL)
    Q_PROPERTY(QString audioCodecName READ audioCodecName WRITE setAudioCodecName NOTIFY audioCodecNameChanged FINAL)
    Q_PROPERTY(QString audioFormatName READ audioFormatName WRITE setAudioFormatName NOTIFY audioFormatNameChanged FINAL)

    Q_PROPERTY(QString videoDuration READ videoDuration WRITE setVideoDuration NOTIFY videoDurationChanged FINAL)
    Q_PROPERTY(int videoSampleRate READ videoSampleRate WRITE setVideoSampleRate NOTIFY videoSampleRateChanged FINAL)
    Q_PROPERTY(int64_t videoBitRate READ videoBitRate WRITE setVideoBitRate NOTIFY videoBitRateChanged FINAL)
    Q_PROPERTY(QString videoCodecName READ videoCodecName WRITE setVideoCodecName NOTIFY videoCodecNameChanged FINAL)
    Q_PROPERTY(QString videoFormatName READ videoFormatName WRITE setVideoFormatName NOTIFY videoFormatNameChanged FINAL)
    Q_PROPERTY(int videoWidth READ videoWidth WRITE setVideoWidth NOTIFY videoWidthChanged)
    Q_PROPERTY(int videoHeight READ videoHeight WRITE setVideoHeight NOTIFY videoHeightChanged)


public:
    // FileMetaData() = default;
    explicit FileMetaData(QObject *parent = nullptr);
    inline static FileMetaData *create(QQmlEngine *engine, QJSEngine *scriptEngine);

    QString path() const;
    void setPath(const QString &newPath);

    QString fileName() const;
    void setFileName(const QString &newFileName);

    qint64 fileSize() const;
    void setFileSize(qint64 newFileSize);

    bool isPlainText() const;
    void setIsPlainText(bool newIsPlainText);

    bool isImage() const;
    void setIsImage(bool newIsImage);

    bool isAudio() const;
    void setIsAudio(const bool &newIsAudio);

    QString icon() const;
    void setIcon(const QString &newIcon);

    QString textContent() const;
    void setTextContent(const QString &newTextContent);

    QString formattedSize() const;
    void setFormattedSize(const QString &newFormattedSize);

    QString md5Hash() const;
    void setMd5Hash(const QString &newMd5Hash);

    QString sha256Hash() const;
    void setSha256Hash(const QString &newSha256Hash);

    QString createdTime() const;
    void setCreatedTime(const QString &newCreatedTime);

    QString modifiedTime() const;
    void setModifiedTime(const QString &newModifiedTime);

    QString user() const;
    void setUser(const QString &newUser);

    QString group() const;
    void setGroup(const QString &newGroup);

    int imageWidth() const;
    void setImageWidth(const int &newImageWidth);

    int imageHeight() const;
    void setImageHeight(const int &newImageHeight);

    int depth() const;
    void setDepth(const int &newDepth);

    QString audioDuration() const;
    void setAudioDuration(const QString &newAudioDuration);

    int audioSampleRate() const;
    void setAudioSampleRate(const int &newAudioSampleRate);

    int audioChannels() const;
    void setAudioChannels(const int &newAudioChannels);

    int64_t audioBitRate() const;
    void setAudioBitRate(const int64_t &newAudioBitRate);

    QString audioCodecName() const;
    void setAudioCodecName(const QString &newAudioCodecName);

    QString audioFormatName() const;
    void setAudioFormatName(const QString &newAudioFormatName);

    QString videoFormatName() const;
    void setVideoFormatName(const QString &newVideoFormatName);

    QString videoCodecName() const;
    void setVideoCodecName(const QString &newVideoCodecName);

    int64_t videoBitRate() const;
    void setVideoBitRate(const int64_t &newVideoBitRate);

    int videoSampleRate() const;
    void setVideoSampleRate(const int &newVideoSampleRate);

    QString videoDuration() const;
    void setVideoDuration(const QString &newVideoDuration);

    bool isVideo() const;
    void setIsVideo(const bool &newIsVideo);

    int videoHeight() const;
    void setVideoHeight(const int &newVideoHeight);

    int videoWidth() const;
    void setVideoWidth(const int &newVideoWidth);

    bool isArchive() const;
    void setIsArchive(const bool &newIsArchive);


signals:
    void pathChanged();
    void fileNameChanged();
    void fileSizeChanged();
    void isPlainTextChanged();
    void isImageChanged();
    void iconChanged();
    void textContentChanged();
    void formattedSizeChanged();
    void md5HashChanged();
    void sha256HashChanged();
    void createdTimeChanged();
    void modifiedTimeChanged();
    void userChanged();
    void groupChanged();
    void imageWidthChanged();
    void imageHeightChanged();
    void depthChanged();
    void isAudioChanged();
    void audioDurationChanged();
    void audioSampleRateChanged();
    void audioChannelsChanged();
    void audioBitRateChanged();
    void audioCodecNameChanged();
    void audioFormatNameChanged();
    void videoFormatNameChanged();
    void videoCodecNameChanged();
    void videoBitRateChanged();
    void videoSampleRateChanged();
    void videoDurationChanged();
    void isVideoChanged();
    void videoWidthChanged();
    void videoHeightChanged();
    void isArchiveChanged();

private:

    const static QStringList s_archive_mimetypes;

    QString m_path;
    QString m_fileName;
    qint64 m_fileSize;
    bool m_isPlainText;
    bool m_isImage;
    QString m_icon;
    QString m_textContent;
    QString m_formattedSize;
    QString m_md5Hash;
    QString m_sha256Hash;
    QString m_createdTime;
    QString m_modifiedTime;
    QString m_user;
    QString m_group;
    int m_imageWidth;
    int m_imageHeight;
    int m_depth;
    bool m_isAudio;
    QString m_audioDuration;
    int m_audioSampleRate;
    int m_audioChannels;
    int64_t m_audioBitRate;
    QString m_audioCodecName;
    QString m_audioFormatName;
    QString m_videoFormatName;
    QString m_videoCodecName;
    int64_t m_videoBitRate;
    int m_videoSampleRate;
    QString m_videoDuration;
    bool m_isVideo;
    int m_videoHeight;
    int m_videoWidth;
    bool m_isArchive;
};

#endif // FILEMETADATA_H
