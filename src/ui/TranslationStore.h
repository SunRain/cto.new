#ifndef TRANSLATIONSTORE_H
#define TRANSLATIONSTORE_H

#include <QObject>
#include <QString>
#include <QLocale>

class TranslationStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLocale READ currentLocale NOTIFY currentLocaleChanged)

public:
    explicit TranslationStore(QObject *parent = nullptr);
    ~TranslationStore() override = default;

    QString currentLocale() const;

    Q_INVOKABLE QString translate(const QString &key) const;
    Q_INVOKABLE void setLocale(const QString &locale);

signals:
    void currentLocaleChanged();

private:
    QLocale m_locale;
};

#endif
