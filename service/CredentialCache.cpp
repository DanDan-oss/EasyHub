#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <Windows.h>
#include <QDebug>
#include "CredentialCache.h"

namespace {
constexpr int CacheVersion = 1;
QString credentialDirectory()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/credentials";
    QDir directory;
    if(!directory.exists(path))
        directory.mkpath(path);
    return path;
}
void clearSensitiveData(QByteArray& data)
{
    if(data.isEmpty())
        return;
    SecureZeroMemory(data.data(), static_cast<SIZE_T>(data.size()));
    data.clear();
}
}

std::optional<QVariantMap> CredentialCache::loadLoginParameters(ProviderType type) const
{
    if(type == ProviderType::Unknown)
        return {};
    auto plainText = loadEncryptedData(accountsPath(), type);
    if(!plainText)
        return {};
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(*plainText, &parseError);
    clearSensitiveData(*plainText);
    if(parseError.error != QJsonParseError::NoError)
        return {};
    if(!document.isObject())
        return {};
    const QVariantMap parameters = document.object().toVariantMap();
    if(parameters.isEmpty())
        return {};
    return parameters;
}

bool CredentialCache::saveLoginParameters(ProviderType type, const QVariantMap& parameters)
{
    if(type == ProviderType::Unknown)
        return false;
    if(parameters.isEmpty())
        return false;
    const QJsonObject object = QJsonObject::fromVariantMap(parameters);
    QByteArray plainText = QJsonDocument(object).toJson(QJsonDocument::Compact);
    if(plainText.isEmpty())
        return false;
    const bool saved = saveEncryptedData(accountsPath(), type, plainText);
    clearSensitiveData(plainText);
    return saved;
}

std::optional<AccessCredential> CredentialCache::loadAccessCredential(ProviderType type) const
{
    if(type == ProviderType::Unknown)
        return {};
    auto plainText = loadEncryptedData(tokensPath(), type);
    if(!plainText)
        return {};
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(*plainText, &parseError);
    clearSensitiveData(*plainText);
    if(parseError.error != QJsonParseError::NoError)
        return {};
    if(!document.isObject())
        return {};
    const QJsonObject object = document.object();
    const QString accessToken = object.value("accessToken").toString();
    const QDateTime expiresAt = QDateTime::fromString(object.value("expiresAt").toString(), Qt::ISODateWithMs);
    const QString  region = object.value("region").toString();
    if(accessToken.isEmpty())
        return {};
    if(!expiresAt.isValid())
        return {};
    if(region.isEmpty())
        return {};
    AccessCredential credential;
    credential.accessToken = accessToken;
    credential.expiresAt = expiresAt;
    credential.region = region;
    return credential;
}

bool CredentialCache::saveAccessCredential(ProviderType type, const AccessCredential& credential)
{
    if(type == ProviderType::Unknown)
        return false;
    if(credential.accessToken.isEmpty())
        return false;
    if(!credential.expiresAt.isValid())
        return false;
    if(credential.region.isEmpty())
        return false;
    QJsonObject object;
    object.insert("accessToken", credential.accessToken);
    object.insert("expiresAt", credential.expiresAt.toUTC().toString(Qt::ISODateWithMs));
    object.insert("region", credential.region);
    QByteArray plainText = QJsonDocument(object).toJson(QJsonDocument::Compact);
    if(plainText.isEmpty())
        return false;
    const bool saved = saveEncryptedData(tokensPath(), type, plainText);
    clearSensitiveData(plainText);
    return saved;
}

bool CredentialCache::clearLoginParameters(ProviderType type)
{
    return clearProviderData(accountsPath(), type);
}

bool CredentialCache::clearAccessCredential(ProviderType type)
{
    return clearProviderData(tokensPath(), type);
}

QString CredentialCache::accountsPath() const
{
    const QString path = credentialDirectory() + "/accounts.json";
    qDebug() << "Accounts cache path: " <<path;
    return path;
}

QString CredentialCache::tokensPath() const
{
    const QString path = credentialDirectory()+"/tokens.json";
    qDebug() << "token cache path: " <<path;
    return path;
}


QJsonObject CredentialCache::readRoot(const QString& path) const
{
    QFile file(path);
    if(!file.exists())
    {
        QJsonObject root;
        root.insert("version", CacheVersion);
        root.insert("providers", QJsonObject());
        return root;
    }
    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open credential cache:" + file.errorString();
        return {};
    }
    const QByteArray data = file.readAll();
    const QJsonDocument document =  QJsonDocument::fromJson(data);
    if(!document.isObject())
        return {};
    return document.object();
}


bool CredentialCache::writeRoot(const QString& path, const QJsonObject& root) const
{
    QSaveFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open credential cache:" << file.errorString();
        return false;
    }
    const QByteArray data = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if(file.write(data) != data.size())
    {
        qWarning() << "Failed to open write credential cache:" << file.errorString();
        file.cancelWriting();
        return false;
    }
    if(!file.commit())
    {
        qWarning() << "Failed to commit credential cache: " << file.errorString();
        return false;
    }
    return true;
}

 bool CredentialCache::saveEncryptedData(const QString& path, ProviderType type, const QByteArray& plainText)
{
     if(type == ProviderType::Unknown)
        return false;
     if(plainText.isEmpty())
         return false;
     auto masterKey = m_masterKeyStore.loadOrCreate();
     if(!masterKey)
         return false;
     const auto encryptedData = m_cipher.encrypt(plainText, *masterKey);
     m_masterKeyStore.clear(*masterKey);
     if(!encryptedData)
         return false;
     QJsonObject root = readRoot(path);
     QJsonObject providers = root.value("providers").toObject();
     const QString providerId = providerTypeToString(type);
     providers.insert(providerId, encryptedDataToJson(*encryptedData));
     root.insert("version", CacheVersion);
     root.insert("providers", providers);
     return writeRoot(path, root);

}

std::optional<QByteArray> CredentialCache::loadEncryptedData(const QString& path, ProviderType type) const
{
    if(type == ProviderType::Unknown)
        return {};
    qDebug() << "Loading encrypted credential: " << path << providerTypeToString(type);
    const QJsonObject root = readRoot(path);
    if(root.isEmpty())
    {
        qWarning() << "Credential root is empty: " << path;
        return {};
    }
    const QJsonObject providers = root.value("providers").toObject();
    const QString providersId = providerTypeToString(type);
    const QJsonValue providerValue = providers.value(providersId);
    if(!providerValue.isObject())
    {
        qWarning() << "Provider credential entry not found: " << providersId;
        return {};
    }
    const auto encryptedData = encryptedDataFromJson(providerValue.toObject());
    if(!encryptedData)
    {
        qWarning() << "Invalid encrypted credential data: " << providersId;
        return {};
    }
    auto masterKey = m_masterKeyStore.load();
    if(!masterKey)
    {
        qWarning() << "Master key not found: ";
        return {};
    }
    auto plainText = m_cipher.decrypt(*encryptedData, *masterKey);
    m_masterKeyStore.clear(*masterKey);
    if(!plainText)
    {
        qWarning() << "Credential decryption Failed: " << providersId;
        return {};
    }
     qDebug() << "Credential decryption successfully: " << providersId;
    return plainText;
}

bool CredentialCache::clearProviderData(const QString& path, ProviderType type)
{
    if(type == ProviderType::Unknown)
        return false;
    QFile file(path);
    if(!file.exists())
        return true;
    QJsonObject root = readRoot(path);
    QJsonObject providers = root.value("providers").toObject();
    const QString providerId = providerTypeToString(type);
    if(!providers.contains(providerId))
        return true;
    providers.remove(providerId);
    root.insert("providers", providers);
    return writeRoot(path, root);
}


QJsonObject CredentialCache::encryptedDataToJson(const EncryptedData& data) const
{
    QJsonObject object;
    object.insert("nonce", QString::fromLatin1(data.nonce.toBase64()));
    object.insert("cipherText", QString::fromLatin1(data.cipherText.toBase64()));
    object.insert("tag", QString::fromLatin1(data.tag.toBase64()));
    return object;
}

std::optional<EncryptedData> CredentialCache::encryptedDataFromJson(const QJsonObject& object) const
{
    const QByteArray nonce = QByteArray::fromBase64(object.value("nonce").toString().toLatin1());
    const QByteArray cipherText = QByteArray::fromBase64(object.value("cipherText").toString().toLatin1());
    const QByteArray tag = QByteArray::fromBase64(object.value("tag").toString().toLatin1());
    if(nonce.isEmpty())
        return {};
    if(cipherText.isEmpty())
        return {};
    if(tag.isEmpty())
        return {};

    EncryptedData data;
    data.nonce = nonce;
    data.cipherText = cipherText;
    data.tag = tag;
    return data;
}
