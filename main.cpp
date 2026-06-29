// #include "filesystemmodel.h"
// #include "iconimageprovider.h"

#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFontDatabase>

#include "iconimageprovider.h"
// #include <fstab.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QIcon::setThemeName("dataicontheme");
    app.setOrganizationName("fscleaner");
    app.setOrganizationDomain("zaval.me");
    app.setApplicationName("FSCleaner");

    QIcon::setThemeSearchPaths(QStringList() << ":/icons");
    QIcon::setThemeName("dataicontheme");

    if (const int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono.ttf"); fontId != -1) {
        qDebug() << "JetBrainsMono.ttf loaded";
    }
    if (const int fontId = QFontDatabase::addApplicationFont(":/fonts/Inter.ttf"); fontId != -1) {
        qDebug() << "Inter.ttf loaded";
        const QString familyName = QFontDatabase::applicationFontFamilies(fontId).at(0);

#ifdef Q_OS_MAC
        // Overwrite the global app default font for all Qt Quick components
        app.setFont(QFont(familyName, 14, 500));
#else
        app.setFont(QFont(familyName, 12, 500));
#endif

    } else {
        qDebug() << "Can't load font";
    }

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/qt/qml/");

    engine.addImageProvider(QLatin1String("theme"), new IconImageProvider);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("fsCleaner", "Main");

    return app.exec();
}
