#pragma once

#include <QtGlobal>
#include <memory>

namespace Runtime {

struct ProcessMetrics {
    qint64 pid = 0;
    double cpuPercent = 0.0;
    double gpuPercent = 0.0;
    double ramMb = 0.0;
    double ramPercent = 0.0;
    double temperatureC = 0.0;
    double gpuTemperatureC = 0.0;
    double powerWatts = 0.0;
    double fps = 0.0;
    bool valid = false;
};

class ProcessMetricsProvider {
public:
    virtual ~ProcessMetricsProvider() = default;
    virtual ProcessMetrics metricsForPid(qint64 pid) = 0;
};

std::shared_ptr<ProcessMetricsProvider> createSystemMetricsProvider();

} // namespace Runtime
