// #include "filesystemmodel.h"
// #include "iconimageprovider.h"

#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

// #include <fstab.h>

int main(int argc, char *argv[])
{

    // QMap<QString, QString> fsMap{};

    // auto fs = getfsent();

    // do {
    //     QString fsSpec = QString::fromLocal8Bit(fs->fs_spec);
    //     QString fsFile = QString::fromLocal8Bit(fs->fs_file);
    //     qDebug() << fsSpec << fsFile;
    //     fsMap.insert(fsSpec, fsFile);
    //     fs = getfsent();
    // } while (fs != nullptr);


    // QQuickStyle::setStyle("Fusion");
    // qputenv("QT_QUICK_CONTROLS_IGNORE_CUSTOMIZATION_WARNINGS", "1");
    QApplication app(argc, argv);
    QIcon::setThemeName("dataicontheme");
    app.setOrganizationName("fscleaner");
    app.setOrganizationDomain("zaval.me");
    app.setApplicationName("FSCleaner");


    // FileSystemModel fileSystemModel; // Create instance of our model

    QQmlApplicationEngine engine;

    // engine.rootContext()->setContextProperty("fileSystemModel", &fileSystemModel);
    // engine.addImageProvider(QLatin1String("themedicons"), new IconImageProvider);




    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("fsCleaner", "Main");

    return app.exec();
}
