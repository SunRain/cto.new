#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QIcon>
#include <QtQml/qqml.h>

#include "ui/NavigationModel.h"
#include "ui/DataModel.h"
#include "ui/TranslationStore.h"

int main(int argc, char *argv[])
{
    qputenv("QSG_RENDER_LOOP", QByteArrayLiteral("basic"));

    QGuiApplication app(argc, argv);

    app.setOrganizationName("QtQuickApp");
    app.setOrganizationDomain("qtquickapp.local");
    app.setApplicationName("QtQuickApp");
    app.setApplicationVersion("1.0.0");

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL); 
    QQuickWindow::setDefaultAlphaBuffer(true);

    qmlRegisterSingletonType<NavigationModel>("App.Models", 1, 0, "NavigationModel",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new NavigationModel();
        }
    );

    qmlRegisterSingletonType<DataModel>("App.Models", 1, 0, "DataModel",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new DataModel();
        }
    );

    qmlRegisterSingletonType<TranslationStore>("App.Models", 1, 0, "TranslationStore",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new TranslationStore();
        }
    );

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qml/AppShell.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
