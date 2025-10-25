#include "ProcessMetricsProvider.hpp"

#include <QDateTime>
#include <QFile>
#include <QHash>
#include <QRegularExpression>
#include <QTextStream>

#include <unistd.h>

namespace Runtime {

namespace {
constexpr double DEFAULT_FPS_FALLBACK = 60.0;
}

class LinuxMetricsProvider : public ProcessMetricsProvider {
public:
    LinuxMetricsProvider();
    ~LinuxMetricsProvider() override = default;

    ProcessMetrics metricsForPid(qint64 pid) override;

private:
    double readCpuUsagePercent(qint64 pid);
    double readGpuUsagePercent();
    double readRamUsageMb(qint64 pid) const;
    double readTemperatureC() const;
    double readGpuTemperatureC() const;
    double readPowerWatts() const;
    double readFps(qint64 pid) const;

    double totalMemoryMb() const;

    struct CpuSample {
        qint64 processTicks = 0;
        qint64 timestampMs = 0;
    };

    mutable QHash<qint64, CpuSample> m_cpuSamples;
    double m_totalMemoryMb = 0.0;
};

LinuxMetricsProvider::LinuxMetricsProvider()
{
    QFile memInfo("/proc/meminfo");
    if (memInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&memInfo);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            if (line.startsWith("MemTotal:")) {
                const QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 2) {
                    bool ok = false;
                    double kb = parts[1].toDouble(&ok);
                    if (ok) {
                        m_totalMemoryMb = kb / 1024.0;
                    }
                }
                break;
            }
        }
    }
}

ProcessMetrics LinuxMetricsProvider::metricsForPid(qint64 pid)
{
    ProcessMetrics metrics;
    metrics.pid = pid;

    QFile statFile(QStringLiteral("/proc/%1/stat").arg(pid));
    if (!statFile.exists() || !statFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return metrics;
    }
    statFile.close();

    metrics.cpuPercent = readCpuUsagePercent(pid);
    metrics.gpuPercent = readGpuUsagePercent();
    metrics.ramMb = readRamUsageMb(pid);
    metrics.temperatureC = readTemperatureC();
    metrics.gpuTemperatureC = readGpuTemperatureC();
    metrics.powerWatts = readPowerWatts();
    metrics.fps = readFps(pid);
    if (m_totalMemoryMb > 0.0) {
        metrics.ramPercent = (metrics.ramMb / m_totalMemoryMb) * 100.0;
    }
    metrics.valid = true;

    return metrics;
}

double LinuxMetricsProvider::readCpuUsagePercent(qint64 pid)
{
    QFile statFile(QStringLiteral("/proc/%1/stat").arg(pid));
    if (!statFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0.0;
    }

    const QString statLine = QTextStream(&statFile).readLine();
    statFile.close();

    const QStringList fields = statLine.split(' ');
    if (fields.size() < 15) {
        return 0.0;
    }

    const qint64 utime = fields[13].toLongLong();
    const qint64 stime = fields[14].toLongLong();
    const qint64 totalTicks = utime + stime;

    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    const auto previous = m_cpuSamples.value(pid);

    double percent = 0.0;
    if (previous.timestampMs > 0 && totalTicks >= previous.processTicks) {
        const qint64 ticksDelta = totalTicks - previous.processTicks;
        const qint64 timeDeltaMs = nowMs - previous.timestampMs;
        const long ticksPerSecond = sysconf(_SC_CLK_TCK);
        if (ticksPerSecond > 0 && timeDeltaMs > 0) {
            const double seconds = timeDeltaMs / 1000.0;
            percent = (ticksDelta / static_cast<double>(ticksPerSecond)) / seconds * 100.0;
        }
    }

    m_cpuSamples.insert(pid, {totalTicks, nowMs});
    if (percent < 0.0) {
        percent = 0.0;
    }
    if (percent > 100.0) {
        percent = 100.0;
    }
    return percent;
}

double LinuxMetricsProvider::readGpuUsagePercent()
{
    QFile gpuBusy(QStringLiteral("/sys/class/drm/card0/device/gpu_busy_percent"));
    if (!gpuBusy.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0.0;
    }

    const QString content = QTextStream(&gpuBusy).readAll().trimmed();
    gpuBusy.close();

    bool ok = false;
    const double value = content.toDouble(&ok);
    if (!ok) {
        return 0.0;
    }
    if (value < 0.0) {
        return 0.0;
    }
    if (value > 100.0) {
        return 100.0;
    }
    return value;
}

double LinuxMetricsProvider::readRamUsageMb(qint64 pid) const
{
    QFile statusFile(QStringLiteral("/proc/%1/status").arg(pid));
    if (!statusFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0.0;
    }

    QTextStream in(&statusFile);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith("VmRSS:")) {
            const QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                bool ok = false;
                const double kb = parts[1].toDouble(&ok);
                if (ok) {
                    return kb / 1024.0;
                }
            }
        }
    }

    return 0.0;
}

double LinuxMetricsProvider::readTemperatureC() const
{
    const QStringList candidates = {
        QStringLiteral("/sys/class/thermal/thermal_zone0/temp"),
        QStringLiteral("/sys/class/hwmon/hwmon0/temp1_input"),
        QStringLiteral("/sys/class/hwmon/hwmon1/temp1_input")
    };

    for (const QString& path : candidates) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString content = QTextStream(&file).readAll().trimmed();
            file.close();

            bool ok = false;
            const double milli = content.toDouble(&ok);
            if (ok) {
                return milli / 1000.0;
            }
        }
    }

    return 0.0;
}

double LinuxMetricsProvider::readGpuTemperatureC() const
{
    const QStringList candidates = {
        QStringLiteral("/sys/class/drm/card0/device/hwmon/hwmon0/temp1_input"),
        QStringLiteral("/sys/class/drm/card0/device/hwmon/hwmon1/temp1_input"),
        QStringLiteral("/sys/class/hwmon/hwmon2/temp1_input")
    };

    for (const QString& path : candidates) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString content = QTextStream(&file).readAll().trimmed();
            file.close();

            bool ok = false;
            const double milli = content.toDouble(&ok);
            if (ok) {
                return milli / 1000.0;
            }
        }
    }

    return readTemperatureC();
}

double LinuxMetricsProvider::readPowerWatts() const
{
    const QStringList candidates = {
        QStringLiteral("/sys/class/power_supply/BAT0/power_now"),
        QStringLiteral("/sys/class/hwmon/hwmon0/power1_input")
    };

    for (const QString& path : candidates) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString content = QTextStream(&file).readAll().trimmed();
            file.close();

            bool ok = false;
            const double micro = content.toDouble(&ok);
            if (ok) {
                return micro / 1'000'000.0;
            }
        }
    }

    return 0.0;
}

double LinuxMetricsProvider::readFps(qint64 pid) const
{
    Q_UNUSED(pid);
    return DEFAULT_FPS_FALLBACK;
}

double LinuxMetricsProvider::totalMemoryMb() const
{
    return m_totalMemoryMb;
}

std::shared_ptr<ProcessMetricsProvider> createSystemMetricsProvider()
{
    return std::make_shared<LinuxMetricsProvider>();
}

} // namespace Runtime
