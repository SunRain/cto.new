#include "performance.h"
#include <QDebug>

PerformanceMonitor::PerformanceMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &PerformanceMonitor::collectMetrics);
}

void PerformanceMonitor::start()
{
    qInfo() << "Starting performance monitoring...";
    m_timer.start(5000);
}

void PerformanceMonitor::collectMetrics()
{
    emit metricsUpdated(0.0, 0.0, 0.0);
}
