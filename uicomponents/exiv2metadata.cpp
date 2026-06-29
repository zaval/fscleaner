#include "exiv2metadata.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include  <QJsonDocument>
#include <QJsonObject>

#include "exiv2/exif.hpp"

Exiv2ParserThread::Exiv2ParserThread(const QString &filePath, QObject *parent):
    QThread(parent),
    m_filePath(filePath)
{}

void Exiv2ParserThread::run() {
    try {
        auto image = Exiv2::ImageFactory::open(m_filePath.toStdString());
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            qDebug() << "No EXIF metadata found.";
            return;
        }
        emit exiv2Loaded(exifData);
    } catch (Exiv2::Error& e) {
        qDebug() << "Error loading exiv2 data: " << e.what();
    }

}

Exiv2Metadata::Exiv2Metadata(QObject *parent) :
    QObject(parent),
    m_gpsAltitude(0),
    m_gpsLongitudeDecimal(0),
    m_gpsLatitudeDecimal(0),
    m_whiteBalance(0),
    m_exposureMode(0),
    m_pixelYDimension(0),
    m_pixelXDimension(0),
    m_focalLength(0),
    m_flash(false),
    m_subjectDistance(0),
    m_apertureValue(0),
    m_shutterSpeed(0),
    m_fNumber(0),
    m_exposureTime(0),
    m_resolution(0),
    m_exivStringCallbacks{
        {"Exif.Image.Make", [this](const QString &value) { setMake(value); }},
        {"Exif.Image.Model", [this](const QString &value) { setModel(value); }},
        {"Exif.Image.Software", [this](const QString &value) { setSoftware(value); }},
        {"Exif.Photo.DateTimeOriginal", [this](const QString &value) { setDateTime(value); }},
        {"Exif.Photo.OffsetTime", [this](const QString &value) { setOffsetTime(value); }},
        {"Exif.Photo.LensMake", [this](const QString &value) { setLensMake(value); }},
        {"Exif.Photo.LensModel", [this](const QString &value) { setLensModel(value); }},
    },
    m_exivRealCallbacks{
        {"Exif.Photo.SubjectDistance", [this](const qreal &value) { setSubjectDistance(value); }},
        {"Exif.Photo.ApertureValue", [this](const qreal &value) { setApertureValue(value); }},
        {"Exif.Photo.ShutterSpeedValue", [this](const qreal &value) { setShutterSpeed(value); }},
        {"Exif.Photo.FNumber", [this](const qreal &value) { setFNumber(value); }},
        {"Exif.Photo.ExposureTime", [this](const qreal &value) { setExposureTime(value); }},
        {"Exif.Photo.FocalLengthIn35mmFilm", [this](const qreal &value) { setFocalLength(value); }},
        {"Exif.Image.XResolution", [this](const qreal &value) { setResolution(value); }},
    },
    m_exivShortCallbacks{
        {"Exif.Photo.WhiteBalance", [this](const short &value) { setWhiteBalance(value); }},
        {"Exif.Photo.ExposureMode", [this](const short &value) { setExposureMode(value); }},
    }
{}

qreal Exiv2Metadata::gpsAltitude() const {
    return m_gpsAltitude;
}

void Exiv2Metadata::setGpsAltitude(const qreal &newGpsAltitude) {
    if (m_gpsAltitude == newGpsAltitude)
        return;
    m_gpsAltitude = newGpsAltitude;
    emit gpsAltitudeChanged();

}

qreal Exiv2Metadata::gpsLongitudeDecimal() const {
    return m_gpsLongitudeDecimal;
}

void Exiv2Metadata::setGpsLongitudeDecimal(const qreal &newGpsLongitudeDecimal) {
    if (m_gpsLongitudeDecimal == newGpsLongitudeDecimal)
        return;
    m_gpsLongitudeDecimal = newGpsLongitudeDecimal;
    emit gpsLongitudeDecimalChanged();

}

qreal Exiv2Metadata::gpsLatitudeDecimal() const {
    return m_gpsLatitudeDecimal;
}

void Exiv2Metadata::setGpsLatitudeDecimal(const qreal &newGpsLatitudeDecimal) {
    if (m_gpsLatitudeDecimal == newGpsLatitudeDecimal)
        return;
    m_gpsLatitudeDecimal = newGpsLatitudeDecimal;
    emit gpsLatitudeDecimalChanged();

}

QString Exiv2Metadata::gpsLongitude() const {
    return m_gpsLongitude;
}

void Exiv2Metadata::setGpsLongitude(const QString &newGpsLongitude) {
    if (m_gpsLongitude == newGpsLongitude)
        return;
    m_gpsLongitude = newGpsLongitude;
    emit gpsLongitudeChanged();

}

QString Exiv2Metadata::gpsLatitude() const {
    return m_gpsLatitude;
}

void Exiv2Metadata::setGpsLatitude(const QString &newGpsLatitude) {
    if (m_gpsLatitude == newGpsLatitude)
        return;
    m_gpsLatitude = newGpsLatitude;
    emit gpsLatitudeChanged();

}

short Exiv2Metadata::whiteBalance() const {
    return m_whiteBalance;
}

void Exiv2Metadata::setWhiteBalance(const short &newWhiteBalance) {
    if (m_whiteBalance == newWhiteBalance)
        return;
    m_whiteBalance = newWhiteBalance;
    emit whiteBalanceChanged();

}

short Exiv2Metadata::exposureMode() const {
    return m_exposureMode;
}

void Exiv2Metadata::setExposureMode(const short &newExposureMode) {
    if (m_exposureMode == newExposureMode)
        return;
    m_exposureMode = newExposureMode;
    emit exposureModeChanged();

}

qint64 Exiv2Metadata::pixelYDimension() const {
    return m_pixelYDimension;
}

void Exiv2Metadata::setPixelYDimension(const qint64 &newPixelYDimension) {
    if (m_pixelYDimension == newPixelYDimension)
        return;
    m_pixelYDimension = newPixelYDimension;
    emit pixelYDimensionChanged();

}

qint64 Exiv2Metadata::pixelXDimension() const {
    return m_pixelXDimension;
}

void Exiv2Metadata::setPixelXDimension(const qint64 &newPixelXDimension) {
    if (m_pixelXDimension == newPixelXDimension)
        return;
    m_pixelXDimension = newPixelXDimension;
    emit pixelXDimensionChanged();

}

qreal Exiv2Metadata::focalLength() const {
    return m_focalLength;
}

void Exiv2Metadata::setFocalLength(const qreal &newFocalLength) {
    if (m_focalLength == newFocalLength)
        return;
    m_focalLength = newFocalLength;
    emit focalLengthChanged();

}

bool Exiv2Metadata::flash() const {
    return m_flash;
}

void Exiv2Metadata::setFlash(const bool &newFlash) {
    if (m_flash == newFlash)
        return;
    m_flash = newFlash;
    emit flashChanged();

}

qreal Exiv2Metadata::subjectDistance() const {
    return m_subjectDistance;
}

void Exiv2Metadata::setSubjectDistance(const qreal &newSubjectDistance) {
    if (m_subjectDistance == newSubjectDistance)
        return;
    m_subjectDistance = newSubjectDistance;
    emit subjectDistanceChanged();
}

qreal Exiv2Metadata::apertureValue() const {
    return m_apertureValue;
}

void Exiv2Metadata::setApertureValue(const qreal &newApertureValue) {
    if (m_apertureValue == newApertureValue)
        return;
    m_apertureValue = newApertureValue;
    emit apertureValueChanged();
}

qreal Exiv2Metadata::shutterSpeed() const {
    return m_shutterSpeed;
}

void Exiv2Metadata::setShutterSpeed(const qreal &newShutterSpeed) {
    if (m_shutterSpeed == newShutterSpeed)
        return;
    m_shutterSpeed = newShutterSpeed;
    emit shutterSpeedChanged();
}

QString Exiv2Metadata::offsetTime() const {
    return m_offsetTime;
}

void Exiv2Metadata::setOffsetTime(const QString &newOffsetTime) {
    if (m_offsetTime == newOffsetTime)
        return;
    m_offsetTime = newOffsetTime;
    emit offsetTimeChanged();

}

qreal Exiv2Metadata::fNumber() const {
    return m_fNumber;
}

void Exiv2Metadata::setFNumber(const qreal &newFNumber) {
    if (m_fNumber == newFNumber)
        return;
    m_fNumber = newFNumber;
    emit fNumberChanged();

}

qreal Exiv2Metadata::exposureTime() const {
    return m_exposureTime;
}

void Exiv2Metadata::setExposureTime(const qreal &newExposureTime) {
    if (m_exposureTime == newExposureTime)
        return;
    m_exposureTime = newExposureTime;
    emit exposureTimeChanged();

}

QString Exiv2Metadata::dateTime() const {
    return m_dateTime;
}

void Exiv2Metadata::setDateTime(const QString &newDateTime) {
    if (m_dateTime == newDateTime)
        return;
    m_dateTime = newDateTime;
    emit dateTimeChanged();

}

qreal Exiv2Metadata::resolution() const {
    return m_resolution;
}

void Exiv2Metadata::setResolution(const qreal &newResolution) {
    if (m_resolution == newResolution)
        return;
    m_resolution = newResolution;
    emit resolutionChanged();

}

QString Exiv2Metadata::software() const {
    return m_software;
}

void Exiv2Metadata::setSoftware(const QString &newSoftware) {
    if (m_software == newSoftware)
        return;
    m_software = newSoftware;
    emit softwareChanged();

}

QString Exiv2Metadata::model() const {
    return m_model;
}

void Exiv2Metadata::setModel(const QString &newModel) {
    if (m_model == newModel)
        return;
    m_model = newModel;
    emit modelChanged();

}

QString Exiv2Metadata::make() const {
    return m_make;
}

void Exiv2Metadata::setMake(const QString &newMake) {
    if (m_make == newMake)
        return;
    m_make = newMake;
    emit makeChanged();

}

QString Exiv2Metadata::filePath() const {
    return m_filePath;
}

void Exiv2Metadata::setFilePath(const QString &newFilePath) {
    if (m_filePath == newFilePath)
        return;
    m_filePath = newFilePath;
    emit filePathChanged();

    const auto exiv2Thread = new Exiv2ParserThread(m_filePath);
    connect(exiv2Thread, &Exiv2ParserThread::finished, exiv2Thread, &Exiv2Metadata::deleteLater);
    connect(exiv2Thread, &Exiv2ParserThread::exiv2Loaded, this, &Exiv2Metadata::exiv2Loaded);
    exiv2Thread->start();

}

QString Exiv2Metadata::lensModel() const {
    return m_lensModel;
}

void Exiv2Metadata::setLensModel(const QString &newLensModel) {
    if (m_lensModel == newLensModel)
        return;
    m_lensModel = newLensModel;
    emit lensModelChanged();

}

QString Exiv2Metadata::lensMake() const {
    return m_lensMake;
}

void Exiv2Metadata::setLensMake(const QString &newLensMake) {
    if (m_lensMake == newLensMake)
        return;
    m_lensMake = newLensMake;
    emit lensMakeChanged();

}

QString Exiv2Metadata::gpsAddress() const {
    return m_gpsAddress;
}

void Exiv2Metadata::setGpsAddress(const QString &newGpsAddress) {
    if (m_gpsAddress == newGpsAddress)
        return;
    m_gpsAddress = newGpsAddress;
    emit gpsAddressChanged();
}

void Exiv2Metadata::loadAddress(const qreal lat, const qreal lon) {
    const auto url = QString("https://nominatim.openstreetmap.org/reverse?lat=%1&lon=%2&format=json").arg(lat).arg(lon);
    // QNetworkAccessManager manager;
    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/149.0.0.0 Safari/537.36");
    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        setGpsAddress(jsonObj["display_name"].toString());
    });
    connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError error) {
        qDebug() << "Error loading address: " << error;
    });
    connect(reply, &QNetworkReply::sslErrors, this, [reply](const QList<QSslError> &errors) {
        qDebug() << "SSL errors: " << errors;
    });

    connect(reply, &QNetworkReply::redirected, this, [](const QUrl &url) {
        qDebug() << "Redirected to: " << url;
    });

}

void Exiv2Metadata::exiv2Loaded(const Exiv2::ExifData &exiv2Data) {

    for (auto const& [key, func]: m_exivStringCallbacks.asKeyValueRange()) {
        if (const auto it = exiv2Data.findKey(Exiv2::ExifKey(key.toStdString())); it != exiv2Data.end()) {
            func(QString::fromUtf8(it->value().toString().c_str()));
        }
    }

    for (auto const& [key, func]: m_exivRealCallbacks.asKeyValueRange()) {
        if (const auto it = exiv2Data.findKey(Exiv2::ExifKey(key.toStdString())); it != exiv2Data.end()) {
            const auto rationalValue = it->value().toRational();
            const qreal value = static_cast<qreal>(rationalValue.first) / static_cast<qreal>(rationalValue.second);
            func(value);
        }
    }

    for (auto const& [key, func]: m_exivShortCallbacks.asKeyValueRange()) {
        if (const auto it = exiv2Data.findKey(Exiv2::ExifKey(key.toStdString())); it != exiv2Data.end()) {
            const auto value = static_cast<short>(it->value().toInt64());
            func(value);
        }
    }

    std::unique_ptr<double> latDecimal;
    std::unique_ptr<double> lonDecimal;
    if (const auto it = exiv2Data.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude")); it != exiv2Data.end()) {
        const auto degree = it->toRational(0);
        const auto min = it->toRational(1);
        const auto sec = it->toRational(2);

        QString latRef = "N";
        if (const auto it2 = exiv2Data.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef")); it2 != exiv2Data.end()) {
            latRef = QString::fromUtf8(it2->value().toString().c_str());
        }

        std::stringstream ss;
        ss << latRef.toStdString()
            << (static_cast<double>(degree.first) / degree.second)
            << "˚"
            << (static_cast<double>(min.first) / min.second)
            << "\""
            << (static_cast<double>(sec.first) / sec.second)
            << "'";

        setGpsLatitude(ss.str().c_str());

        latDecimal = std::make_unique<double>(
            latRef.toStdString() == "S" ? -1.0 : 1.0 *
            static_cast<double>(degree.first) / degree.second +
            static_cast<double>(min.first) / min.second / 60 +
            static_cast<double>(sec.first) / sec.second / 3600
        );
    }

    if (const auto it = exiv2Data.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude")); it != exiv2Data.end()) {
        const auto degree = it->toRational(0);
        const auto min = it->toRational(1);
        const auto sec = it->toRational(2);

        QString lonRef = "E";
        if (const auto it2 = exiv2Data.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef")); it2 != exiv2Data.end()) {
            lonRef = QString::fromUtf8(it2->value().toString().c_str());
        }
        std::stringstream ss;
        ss << lonRef.toStdString()
            << (static_cast<double>(degree.first) / degree.second)
            << "˚"
            << (static_cast<double>(min.first) / min.second)
            << "\""
            << (static_cast<double>(sec.first) / sec.second)
            << "'";

        setGpsLongitude(ss.str().c_str());

        lonDecimal = std::make_unique<double>(
            lonRef.toStdString() == "W" ? -1.0 : 1.0 *
            static_cast<double>(degree.first) / degree.second +
            static_cast<double>(min.first) / min.second / 60 +
            static_cast<double>(sec.first) / sec.second / 3600
        );
    }
    if (latDecimal && lonDecimal) {
        setGpsLatitudeDecimal(*latDecimal);
        setGpsLongitudeDecimal(*lonDecimal);
        // qDebug() << "https://nominatim.openstreetmap.org/reverse?lat=" << *latDecimal << "&lon=" << *lonDecimal;
        loadAddress(*latDecimal, *lonDecimal);
    }


}
