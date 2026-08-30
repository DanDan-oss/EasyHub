#pragma once
#include <QHash>
#include <QString>
#include <QDateTime>

#include "../model/ProviderType.h"

struct AccessCredential
{
    QString accessToken;
    QDateTime expiresAt;
};

class CredentialStore
{
public:
    void setAccessToken(ProviderType type, const QString& accessToken, const QDateTime& expiresAt);
    QString accessToken(ProviderType type) const;
    bool isAccessTokenValid(ProviderType type) const;
    void clear(ProviderType type);
private:
    QHash<ProviderType, AccessCredential> m_credentials;
};
