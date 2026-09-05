#pragma once
#include <QHash>
#include <QString>
#include <QDateTime>

#include "../model/ProviderType.h"

struct AccessCredential
{
    QString accessToken;
    QDateTime expiresAt;
    QString region;
};

class CredentialStore
{
public:
    void setCredential(ProviderType type, const AccessCredential& credential);
    const AccessCredential* credential(ProviderType type);
    QString accessToken(ProviderType type) const;
    bool isAccessTokenValid(ProviderType type) const;
    void clear(ProviderType type);
private:
    QHash<ProviderType, AccessCredential> m_credentials;
};
