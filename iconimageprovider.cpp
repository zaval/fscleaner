#include "iconimageprovider.h"

#include <QIcon>

IconImageProvider::IconImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QIcon icon = QIcon::fromTheme(id);
    if (icon.isNull()) {
        qWarning() << "IconImageProvider: Could not find icon" << id;
        // Return a default/error pixmap?
        return QPixmap(requestedSize.isValid() ? requestedSize : QSize(32, 32));
    }

    QSize actualSize = requestedSize.isValid() ? requestedSize : QSize(32, 32);
    if (size) {
        *size = actualSize;
    }
    return icon.pixmap(actualSize);
}
