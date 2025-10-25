#include "runtime/RunningManager.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

using namespace Qt::StringLiterals;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Runtime::RunningManager manager;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("runningManager", &manager);

    const QUrl url(u"qrc:/qt/qml/RuntimeOverlay/RunningOverlay.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
