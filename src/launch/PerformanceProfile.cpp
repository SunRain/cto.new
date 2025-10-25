#include "PerformanceProfile.hpp"

#include <QtGlobal>

namespace Launch {

namespace {
static QString normalizeString(const QString& input)
{
    QString normalized = input.trimmed().toLower();
    return normalized;
}
} // namespace

PerformanceProfile PerformanceProfile::defaultProfile()
{
    PerformanceProfile profile;
    profile.fpsCap = 0;
    profile.scaling = ScalingMode::Native;
    profile.vsync = true;
    profile.windowMode = WindowMode::Fullscreen;
    profile.renderWidth = 0;
    profile.renderHeight = 0;
    profile.outputWidth = 0;
    profile.outputHeight = 0;
    profile.allowTearing = false;
    profile.refreshRate = 0;
    profile.scalingFilter = QStringLiteral("linear");
    profile.fsrSharpness = 5;
    return profile;
}

PerformanceProfile PerformanceProfile::fromVariantMap(const QVariantMap& map)
{
    PerformanceProfile profile = PerformanceProfile::defaultProfile();

    if (map.contains("fpsCap")) {
        profile.fpsCap = map.value("fpsCap").toInt();
    }

    if (map.contains("scaling")) {
        profile.scaling = scalingModeFromString(map.value("scaling").toString());
    }

    if (map.contains("vsync")) {
        profile.vsync = map.value("vsync").toBool();
    }

    if (map.contains("windowMode")) {
        profile.windowMode = windowModeFromString(map.value("windowMode").toString());
    }

    if (map.contains("renderWidth")) {
        profile.renderWidth = map.value("renderWidth").toInt();
    }

    if (map.contains("renderHeight")) {
        profile.renderHeight = map.value("renderHeight").toInt();
    }

    if (map.contains("outputWidth")) {
        profile.outputWidth = map.value("outputWidth").toInt();
    }

    if (map.contains("outputHeight")) {
        profile.outputHeight = map.value("outputHeight").toInt();
    }

    if (map.contains("allowTearing")) {
        profile.allowTearing = map.value("allowTearing").toBool();
    }

    if (map.contains("refreshRate")) {
        profile.refreshRate = map.value("refreshRate").toInt();
    }

    if (map.contains("scalingFilter")) {
        profile.scalingFilter = map.value("scalingFilter").toString();
    }

    if (map.contains("fsrSharpness")) {
        profile.fsrSharpness = map.value("fsrSharpness").toInt();
    }

    return profile;
}

QVariantMap PerformanceProfile::toVariantMap() const
{
    QVariantMap map;
    map["fpsCap"] = fpsCap;
    map["scaling"] = scalingModeToString(scaling);
    map["vsync"] = vsync;
    map["windowMode"] = windowModeToString(windowMode);
    map["renderWidth"] = renderWidth;
    map["renderHeight"] = renderHeight;
    map["outputWidth"] = outputWidth;
    map["outputHeight"] = outputHeight;
    map["allowTearing"] = allowTearing;
    map["refreshRate"] = refreshRate;
    map["scalingFilter"] = scalingFilter;
    map["fsrSharpness"] = fsrSharpness;
    return map;
}

bool PerformanceProfile::isValid() const
{
    if (fpsCap < 0) {
        return false;
    }

    if (renderWidth < 0 || renderHeight < 0 || outputWidth < 0 || outputHeight < 0) {
        return false;
    }

    if (fsrSharpness < 0 || fsrSharpness > 20) {
        return false;
    }

    return true;
}

QString scalingModeToString(ScalingMode mode)
{
    switch (mode) {
    case ScalingMode::Native:
        return QStringLiteral("native");
    case ScalingMode::FSR:
        return QStringLiteral("fsr");
    case ScalingMode::Integer:
        return QStringLiteral("integer");
    default:
        return QStringLiteral("native");
    }
}

ScalingMode scalingModeFromString(const QString& str)
{
    const QString normalized = normalizeString(str);
    if (normalized == QStringLiteral("fsr")) {
        return ScalingMode::FSR;
    }
    if (normalized == QStringLiteral("integer")) {
        return ScalingMode::Integer;
    }
    return ScalingMode::Native;
}

QString windowModeToString(WindowMode mode)
{
    switch (mode) {
    case WindowMode::Fullscreen:
        return QStringLiteral("fullscreen");
    case WindowMode::Borderless:
        return QStringLiteral("borderless");
    case WindowMode::Windowed:
        return QStringLiteral("windowed");
    default:
        return QStringLiteral("fullscreen");
    }
}

WindowMode windowModeFromString(const QString& str)
{
    const QString normalized = normalizeString(str);
    if (normalized == QStringLiteral("borderless")) {
        return WindowMode::Borderless;
    }
    if (normalized == QStringLiteral("windowed")) {
        return WindowMode::Windowed;
    }
    return WindowMode::Fullscreen;
}

} // namespace Launch
