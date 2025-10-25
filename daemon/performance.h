#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <QObject>
#include <QTimer>

class PerformanceMonitor : public QObject
{
    Q_OBJECT
public:
    explicit PerformanceMonitor(QObject *parent = nullptr);

    void start();

signals:
    void metricsUpdated(double cpuUsage, double gpuUsage, double temperature);

private slots:
    void collectMetrics();

private:
    QTimer m_timer;
};

#endif // PERFORMANCE_H
