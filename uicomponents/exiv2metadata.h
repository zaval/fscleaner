#ifndef FSCLEANER_EXIV2METADATA_H
#define FSCLEANER_EXIV2METADATA_H
#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>
#include <qqmlintegration.h>
#include <QThread>
#include <exiv2/exiv2.hpp>


class Exiv2ParserThread: public QThread {
    Q_OBJECT
public:
    explicit Exiv2ParserThread(const QString &filePath, QObject *parent = nullptr);

signals:
    void exiv2Loaded(const Exiv2::ExifData &exiv2Data);

protected:
    void run() override;

private:
    QString m_filePath;
};

class Exiv2Metadata: public QObject {
    Q_OBJECT

    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

    Q_PROPERTY(QString make READ make WRITE setMake NOTIFY makeChanged)
    Q_PROPERTY(QString model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString software READ software WRITE setSoftware NOTIFY softwareChanged)
    Q_PROPERTY(qreal resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QString dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)
    Q_PROPERTY(QString offsetTime READ offsetTime WRITE setOffsetTime NOTIFY offsetTimeChanged)
    Q_PROPERTY(qreal exposureTime READ exposureTime WRITE setExposureTime NOTIFY exposureTimeChanged)
    Q_PROPERTY(qreal fNumber READ fNumber WRITE setFNumber NOTIFY fNumberChanged)
    Q_PROPERTY(qreal shutterSpeed READ shutterSpeed WRITE setShutterSpeed NOTIFY shutterSpeedChanged)
    Q_PROPERTY(qreal apertureValue READ apertureValue WRITE setApertureValue NOTIFY apertureValueChanged)
    Q_PROPERTY(qreal subjectDistance READ subjectDistance WRITE setSubjectDistance NOTIFY subjectDistanceChanged)
    Q_PROPERTY(bool flash READ flash WRITE setFlash NOTIFY flashChanged)
    Q_PROPERTY(qreal focalLength READ focalLength WRITE setFocalLength NOTIFY focalLengthChanged)
    Q_PROPERTY(qint64 pixelXDimension READ pixelXDimension WRITE setPixelXDimension NOTIFY pixelXDimensionChanged)
    Q_PROPERTY(qint64 pixelYDimension READ pixelYDimension WRITE setPixelYDimension NOTIFY pixelYDimensionChanged)
    Q_PROPERTY(short exposureMode READ exposureMode WRITE setExposureMode NOTIFY exposureModeChanged)
    Q_PROPERTY(short whiteBalance READ whiteBalance WRITE setWhiteBalance NOTIFY whiteBalanceChanged)
    Q_PROPERTY(QString gpsLatitude READ gpsLatitude WRITE setGpsLatitude NOTIFY gpsLatitudeChanged)
    Q_PROPERTY(QString gpsLongitude READ gpsLongitude WRITE setGpsLongitude NOTIFY gpsLongitudeChanged)
    Q_PROPERTY(qreal gpsLatitudeDecimal READ gpsLatitudeDecimal WRITE setGpsLatitudeDecimal NOTIFY gpsLatitudeDecimalChanged)
    Q_PROPERTY(qreal gpsLongitudeDecimal READ gpsLongitudeDecimal WRITE setGpsLongitudeDecimal NOTIFY gpsLongitudeDecimalChanged)
    Q_PROPERTY(qreal gpsAltitude READ gpsAltitude WRITE setGpsAltitude NOTIFY gpsAltitudeChanged)
    Q_PROPERTY(QString lensMake READ lensMake WRITE setLensMake NOTIFY lensMakeChanged)
    Q_PROPERTY(QString lensModel READ lensModel WRITE setLensModel NOTIFY lensModelChanged)
    Q_PROPERTY(QString gpsAddress READ gpsAddress WRITE setGpsAddress NOTIFY gpsAddressChanged)

    QML_ELEMENT

public:
    explicit Exiv2Metadata(QObject *parent = nullptr);

    qreal gpsAltitude() const; // NOLINT(*-use-nodiscard)

    void setGpsAltitude(const qreal &newGpsAltitude);

    qreal gpsLongitudeDecimal() const; // NOLINT(*-use-nodiscard)

    void setGpsLongitudeDecimal(const qreal &newGpsLongitudeDecimal);

    qreal gpsLatitudeDecimal() const; // NOLINT(*-use-nodiscard)

    void setGpsLatitudeDecimal(const qreal &newGpsLatitudeDecimal);

    QString gpsLongitude() const; // NOLINT(*-use-nodiscard)

    void setGpsLongitude(const QString &newGpsLongitude);

    QString gpsLatitude() const; // NOLINT(*-use-nodiscard)

    void setGpsLatitude(const QString &newGpsLatitude);

    short whiteBalance() const; // NOLINT(*-use-nodiscard)

    void setWhiteBalance(const short &newWhiteBalance);

    short exposureMode() const; // NOLINT(*-use-nodiscard)

    void setExposureMode(const short &newExposureMode);

    qint64 pixelYDimension() const; // NOLINT(*-use-nodiscard)

    void setPixelYDimension(const qint64 &newPixelYDimension);

    qint64 pixelXDimension() const; // NOLINT(*-use-nodiscard)

    void setPixelXDimension(const qint64 &newPixelXDimension);

    qreal focalLength() const; // NOLINT(*-use-nodiscard)

    void setFocalLength(const qreal &newFocalLength);

    bool flash() const; // NOLINT(*-use-nodiscard)

    void setFlash(const bool &newFlash);

    qreal subjectDistance() const; // NOLINT(*-use-nodiscard)

    void setSubjectDistance(const qreal &newSubjectDistance);

    qreal apertureValue() const; // NOLINT(*-use-nodiscard)

    void setApertureValue(const qreal &newApertureValue);

    qreal shutterSpeed() const; // NOLINT(*-use-nodiscard)

    void setShutterSpeed(const qreal &newShutterSpeed);

    QString offsetTime() const; // NOLINT(*-use-nodiscard)

    void setOffsetTime(const QString &newOffsetTime);

    qreal fNumber() const; // NOLINT(*-use-nodiscard)

    void setFNumber(const qreal &newFNumber);

    qreal exposureTime() const; // NOLINT(*-use-nodiscard)

    void setExposureTime(const qreal &newExposureTime);

    QString dateTime() const; // NOLINT(*-use-nodiscard)

    void setDateTime(const QString &newDateTime);

    qreal resolution() const; // NOLINT(*-use-nodiscard)

    void setResolution(const qreal &newResolution);

    QString software() const; // NOLINT(*-use-nodiscard)

    void setSoftware(const QString &newSoftware);

    QString model() const; // NOLINT(*-use-nodiscard)

    void setModel(const QString &newModel);

    QString make() const; // NOLINT(*-use-nodiscard)

    void setMake(const QString &newMake);

    QString filePath() const; // NOLINT(*-use-nodiscard)

    void setFilePath(const QString &newFilePath);

    QString lensModel() const; // NOLINT(*-use-nodiscard)

    void setLensModel(const QString &newLensModel);

    QString lensMake() const; // NOLINT(*-use-nodiscard)

    void setLensMake(const QString &newLensMake);

    QString gpsAddress() const; // NOLINT(*-use-nodiscard)

    void setGpsAddress(const QString &newGpsAddress);

signals:
    void modelChanged();
    void softwareChanged();
    void resolutionChanged();
    void gpsAltitudeChanged();
    void makeChanged();
    void gpsLongitudeDecimalChanged();
    void gpsLatitudeDecimalChanged();
    void gpsLongitudeChanged();
    void gpsLatitudeChanged();
    void whiteBalanceChanged();
    void exposureModeChanged();
    void pixelYDimensionChanged();
    void focalLengthChanged();
    void pixelXDimensionChanged();
    void flashChanged();
    void subjectDistanceChanged();
    void apertureValueChanged();
    void shutterSpeedChanged();
    void offsetTimeChanged();
    void fNumberChanged();
    void exposureTimeChanged();
    void dateTimeChanged();
    void filePathChanged();
    void lensModelChanged();
    void lensMakeChanged();
    void gpsAddressChanged();


private:
    qreal m_gpsAltitude;
    qreal m_gpsLongitudeDecimal;
    qreal m_gpsLatitudeDecimal;
    QString m_gpsLongitude;
    QString m_gpsLatitude;
    short m_whiteBalance;
    short m_exposureMode;
    qint64 m_pixelYDimension;
    qint64 m_pixelXDimension;
    qreal m_focalLength;
    bool m_flash;
    qreal m_subjectDistance;
    qreal m_apertureValue;
    qreal m_shutterSpeed;
    QString m_offsetTime;
    qreal m_fNumber;
    qreal m_exposureTime;
    QString m_dateTime;
    qreal m_resolution;
    QString m_software;
    QString m_model;
    QString m_make;
    QString m_filePath;
    QString m_lensModel;
    QString m_lensMake;
    // QStringList m_exifStringkeys;
    QHash<QString, std::function<void(const QString)>> m_exivStringCallbacks;
    QHash<QString, std::function<void(const qreal)>> m_exivRealCallbacks;
    QHash<QString, std::function<void(const short)>> m_exivShortCallbacks;
    QString m_gpsAddress;

    void loadAddress(qreal lat, qreal lon);

    QNetworkAccessManager m_manager;

private slots:
    void exiv2Loaded(const Exiv2::ExifData &exiv2Data);
};


#endif //FSCLEANER_EXIV2METADATA_H
