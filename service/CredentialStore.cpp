#include "CredentialStore.h"

void CredentialStore::setAccessToken(ProviderType type, const QString& accessToken, const QDateTime& expiresAt)
{
    setCredential(type, {accessToken, expiresAt});
}

void CredentialStore::setCredential(ProviderType type, const AccessCredential& credential)
{
    if(type == ProviderType::Unknown)
        return;
    if(credential.accessToken.isEmpty())
        return;
    if(!credential.expiresAt.isValid())
        return;
    const QDateTime safeExpiry = credential.expiresAt.addSecs(-5*60);
    if(QDateTime::currentDateTimeUtc() >= safeExpiry)
        return;
    m_credentials.insert(type, credential);
}

QString CredentialStore::accessToken(ProviderType type) const
{
    auto it = m_credentials.constFind(type);
    if(it == m_credentials.constEnd())
        return {};
    return it->accessToken;
}

bool CredentialStore::isAccessTokenValid(ProviderType type) const
{
    auto it = m_credentials.constFind(type);
    if(it == m_credentials.constEnd())
        return false;
    if(it->accessToken.isEmpty())
        return false;
    if(!it->expiresAt.isValid())
        return false;
    const QDateTime safeExpiry = it->expiresAt.addSecs(-5*60);
    return QDateTime::currentDateTimeUtc() < safeExpiry;
}

void CredentialStore::clear(ProviderType type)
{
   m_credentials.remove(type);
}
