#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "performance.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("gameperf-daemon");
    app.setApplicationVersion("1.0.0");

    qInfo() << "GamePerf Daemon starting...";

    PerformanceMonitor monitor;
    monitor.start();

    qInfo() << "GamePerf Daemon ready";

    return app.exec();
}
