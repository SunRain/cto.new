#pragma once

#include <QString>
#include <QVariantMap>

namespace Launch {

enum class ScalingMode {
    Native,
    FSR,
    Integer
};

enum class WindowMode {
    Fullscreen,
    Borderless,
    Windowed
};

struct PerformanceProfile {
    int fpsCap = 0;
    ScalingMode scaling = ScalingMode::Native;
    bool vsync = true;
    WindowMode windowMode = WindowMode::Fullscreen;
    int renderWidth = 0;
    int renderHeight = 0;
    int outputWidth = 0;
    int outputHeight = 0;
    bool allowTearing = false;
    int refreshRate = 0;
    QString scalingFilter;
    int fsrSharpness = 5;

    static PerformanceProfile fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap() const;
    static PerformanceProfile defaultProfile();

    bool isValid() const;
};

QString scalingModeToString(ScalingMode mode);
ScalingMode scalingModeFromString(const QString& str);
QString windowModeToString(WindowMode mode);
WindowMode windowModeFromString(const QString& str);

} // namespace Launch
