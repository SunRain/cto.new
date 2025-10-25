#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <QObject>
#include <QStringList>

class SourceManager : public QObject
{
    Q_OBJECT

public:
    explicit SourceManager(QObject *parent = nullptr);
    ~SourceManager();

    Q_INVOKABLE bool validateSteamPath(const QString &path);
    Q_INVOKABLE bool validateLegendaryInstallation(const QString &path);
    Q_INVOKABLE bool validateWinePrefix(const QString &path);

    Q_INVOKABLE QStringList autoDetectSteamLibraries();
    Q_INVOKABLE QString autoDetectLegendary();

signals:
    void sourceValidated(const QString &source, bool valid);
};

#endif // SOURCEMANAGER_H
