#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "application.h"
#include "settingsmanager.h"
#include "sourcemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("GamePerf");
    app.setOrganizationDomain("gameperf.example.com");
    app.setApplicationName("GamePerf Manager");
    app.setApplicationVersion("1.0.0");

    Application appLogic;
    SourceManager sourceManager;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("application", &appLogic);
    engine.rootContext()->setContextProperty("sourceManager", &sourceManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
