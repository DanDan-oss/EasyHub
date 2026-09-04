#pragma once
#include <optional>
#include <QByteArray>

struct EncryptedData
{
    QByteArray nonce;
    QByteArray cipherText;
    QByteArray tag;
};

class CredentialCipher
{
public:
    std::optional<EncryptedData> encrypt(const QByteArray& plainText, const QByteArray& key) const;
    std::optional<QByteArray> decrypt(const EncryptedData& encryptedData, const QByteArray& key) const;
private:
    QByteArray generateNonce() const;
};
