#include "CredentialStore.h"

void CredentialStore::setAccessToken(ProviderType type, const QString& accessToken, const QDateTime& expiresAt)
{
    if(type == ProviderType::Unknown)
        return;
    if(accessToken.isEmpty())
        return;
    if(expiresAt.isValid())
        return;

    AccessCredential credential;
    credential.accessToken = accessToken;
    credential.expiresAt = expiresAt;
    m_credentials.insert(type, credential);
}

QString CredentialStore::accessToken(ProviderType type) const
{
    auto it = m_credentials.constFind(type);
    if(it == m_credentials.constEnd())
        return {};
    return it->accessToken;
}

bool  CredentialStore::hasAccessToken(ProviderType type) const
{
    auto it = m_credentials.constFind(type);
    if(it == m_credentials.constEnd())
        return false;
    return !it->accessToken.isEmpty();
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
