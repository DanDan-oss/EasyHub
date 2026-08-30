#include "CredentialStore.h"

void CredentialStore::setAccessToken(ProviderType type, const QString& accessToken)
{
    if(type == ProviderType::Unknown)
        return;
    if(accessToken.isEmpty())
        return;
    m_accessTokens.insert(type, accessToken);
}

QString CredentialStore::accessToken(ProviderType type) const
{
    return m_accessTokens.value(type);
}

bool  CredentialStore::hasAccessToken(ProviderType type) const
{
    return m_accessTokens.contains(type);
}

void CredentialStore::clear(ProviderType type)
{
    m_accessTokens.remove(type);
}
