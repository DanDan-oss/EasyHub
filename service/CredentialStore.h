#pragma once
#include <QHash>
#include <QString>

#include "../model/ProviderType.h"

class CredentialStore
{
public:
    void setAccessToken(ProviderType type, const QString& accessToken);
    QString accessToken(ProviderType type) const;
    bool hasAccessToken(ProviderType type) const;
    void clear(ProviderType type);
private:
    QHash<ProviderType, QString> m_accessTokens;
};
