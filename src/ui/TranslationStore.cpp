#include "TranslationStore.h"

#include <QCoreApplication>

TranslationStore::TranslationStore(QObject *parent)
    : QObject(parent)
    , m_locale(QLocale::system())
{
}

QString TranslationStore::currentLocale() const
{
    return m_locale.name();
}

QString TranslationStore::translate(const QString &key) const
{
    // Placeholder implementation: returns the key for now.
    return QCoreApplication::translate("app", key.toUtf8().constData());
}

void TranslationStore::setLocale(const QString &locale)
{
    const QLocale newLocale(locale);
    if (newLocale == m_locale) {
        return;
    }

    m_locale = newLocale;
    emit currentLocaleChanged();
}
