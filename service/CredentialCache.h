#pragma once

#include <QJsonObject>
#include <QString>
#include <QVariantMap>
#include <optional>
#include "CredentialCipher.h"
#include "CredentialStore.h"
#include "WindowsMasterKeyStore.h"
#include "../model/ProviderType.h"

class CredentialCache
{
public:
    std::optional<QVariantMap> loadLoginParameters(ProviderType type) const;
    bool saveLoginParameters(ProviderType type, const QVariantMap& parameters);
    std::optional<AccessCredential> loadAccessCredential(ProviderType type) const;
    bool saveAccessCredential(ProviderType type, const AccessCredential& credential);
    bool clearLoginParameters(ProviderType type);
    bool clearAccessCredential(ProviderType type);

private:
    QString accountsPath() const;
    QString tokensPath() const;
    QJsonObject readRoot(const QString& path) const;
    bool writeRoot(const QString& path, const QJsonObject& root) const;
    std::optional<QByteArray> loadEncryptedData(const QString& path, ProviderType type) const;
    bool saveEncryptedData(const QString& path, ProviderType type, const QByteArray& plainText);
    bool clearProviderData(const QString& path, ProviderType type);
    QJsonObject encryptedDataToJson(const EncryptedData& data) const;
    std::optional<EncryptedData> encryptedDataFromJson(const QJsonObject& object) const;

private:
    WindowsMasterKeyStore m_masterKeyStore;
    CredentialCipher m_cipher;
};
