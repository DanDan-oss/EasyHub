#include <QDebug>
#include <Windows.h>
#include <bcrypt.h>
#include "CredentialCipher.h"

namespace
{
constexpr qsizetype Aes256KeySize = 32;
constexpr qsizetype GcmNonceSize = 12;
constexpr qsizetype GcmTagSize = 16;
}

std::optional<EncryptedData> CredentialCipher::encrypt(const QByteArray& plainText, const QByteArray& key) const
{
    if(plainText.isEmpty())
        return {};
    if(key.size() != Aes256KeySize)
        return {};
    BCRYPT_ALG_HANDLE algorithmHandle = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithmHandle, BCRYPT_AES_ALGORITHM, nullptr, 0);
    if(status<0)
    {
        qWarning() << "Failed to open AES  algorithm provider";
        return {};
    }
    status = BCryptSetProperty(algorithmHandle, BCRYPT_CHAINING_MODE, reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_GCM)), static_cast<ULONG>(sizeof(BCRYPT_CHAIN_MODE_GCM)), 0);
    if(status<0)
    {
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        qWarning() << "Failed to enable AES-GCM mode";
        return {};
    }
    DWORD keyObjectSize = 0;
    DWORD resultSize = 0;
    status = BCryptGetProperty(algorithmHandle, BCRYPT_OBJECT_LENGTH,  reinterpret_cast<PUCHAR>(&keyObjectSize), sizeof(keyObjectSize), &resultSize, 0);
    if(status<0)
    {
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        return {};
    }
    QByteArray keyObject(static_cast<qsizetype>(keyObjectSize), Qt::Uninitialized);
    BCRYPT_KEY_HANDLE keyHandle = nullptr;
    status = BCryptGenerateSymmetricKey(algorithmHandle, &keyHandle, reinterpret_cast<PUCHAR>(keyObject.data()), keyObjectSize, \
                                        reinterpret_cast<PUCHAR>(const_cast<char*>(key.constData())), static_cast<ULONG>(key.size()), 0);
    if(status<0)
    {
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        return {};
    }
    QByteArray nonce = generateNonce();
    if(nonce.size() != GcmNonceSize)
    {
        BCryptDestroyKey(keyHandle);
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        SecureZeroMemory(keyObject.data(), static_cast<SIZE_T>(keyObject.size()));
        return {};
    }
    QByteArray tag(GcmTagSize, Qt::Uninitialized);
    QByteArray cipherText(plainText.size(), Qt::Uninitialized);
    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
    BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
    authInfo.pbNonce = reinterpret_cast<PUCHAR>(nonce.data());
    authInfo.cbNonce = static_cast<ULONG>(nonce.size());
    authInfo.pbTag = reinterpret_cast<PUCHAR>(tag.data());
    authInfo.cbTag = static_cast<ULONG>(tag.size());
    ULONG encryptedSize = 0;

    status = BCryptEncrypt(keyHandle, reinterpret_cast<PUCHAR>(const_cast<char*>(plainText.constData())), static_cast<ULONG>(plainText.size()), \
             &authInfo, nullptr, 0, reinterpret_cast<PUCHAR>(cipherText.data()), static_cast<ULONG>(cipherText.size()), &encryptedSize, 0);
    BCryptDestroyKey(keyHandle);
    BCryptCloseAlgorithmProvider(algorithmHandle, 0);
    SecureZeroMemory(keyObject.data(), static_cast<SIZE_T>(keyObject.size()));
    if(status < 0)
    {
        qWarning() << "AES-GCM encryption failed";
        return {};
    }
    cipherText.resize(static_cast<qsizetype>(encryptedSize));
    EncryptedData encryptedData;
    encryptedData.nonce = std::move(nonce);
    encryptedData.cipherText = std::move(cipherText);
    encryptedData.tag = std::move(tag);
    return encryptedData;
}

std::optional<QByteArray> CredentialCipher::decrypt(const EncryptedData& encryptedData, const QByteArray& key) const
{
    if(key.size() != Aes256KeySize)
        return {};
    if(encryptedData.nonce.size() != GcmNonceSize)
        return {};
    if(encryptedData.tag.size() != GcmTagSize)
        return {};
    if(encryptedData.cipherText.isEmpty())
        return {};

    BCRYPT_ALG_HANDLE algorithmHandle = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithmHandle, BCRYPT_AES_ALGORITHM, nullptr, 0);
    if(status<0)
    {
        qWarning() << "Failed to open AES  algorithm provider";
        return {};
    }
    status = BCryptSetProperty(algorithmHandle, BCRYPT_CHAINING_MODE, reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_GCM)), static_cast<ULONG>(sizeof(BCRYPT_CHAIN_MODE_GCM)), 0);
    if(status<0)
    {
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        qWarning() << "Failed to enable AES-GCM mode";
        return {};
    }
    DWORD keyObjectSize = 0;
    DWORD resultSize = 0;
    status = BCryptGetProperty(algorithmHandle, BCRYPT_OBJECT_LENGTH,  reinterpret_cast<PUCHAR>(&keyObjectSize), sizeof(keyObjectSize), &resultSize, 0);
    if(status<0)
    {
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        return {};
    }
    QByteArray keyObject(static_cast<qsizetype>(keyObjectSize), Qt::Uninitialized);
    BCRYPT_KEY_HANDLE keyHandle = nullptr;
    status = BCryptGenerateSymmetricKey(algorithmHandle, &keyHandle, reinterpret_cast<PUCHAR>(keyObject.data()), keyObjectSize, \
                                        reinterpret_cast<PUCHAR>(const_cast<char*>(key.constData())), static_cast<ULONG>(key.size()), 0);
    if(status<0)
    {
        SecureZeroMemory(keyObject.data(), static_cast<SIZE_T>(keyObject.size()));
        BCryptCloseAlgorithmProvider(algorithmHandle, 0);
        return {};
    }
    QByteArray nonce = encryptedData.nonce;
    QByteArray tag = encryptedData.tag;
    QByteArray plainText(encryptedData.cipherText.size(), Qt::Uninitialized);

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
    BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
    authInfo.pbNonce = reinterpret_cast<PUCHAR>(nonce.data());
    authInfo.cbNonce = static_cast<ULONG>(nonce.size());
    authInfo.pbTag = reinterpret_cast<PUCHAR>(tag.data());
    authInfo.cbTag = static_cast<ULONG>(tag.size());
    ULONG plainTextSize = 0;

    status = BCryptDecrypt(keyHandle, reinterpret_cast<PUCHAR>(const_cast<char*>(encryptedData.cipherText.constData())), static_cast<ULONG>(encryptedData.cipherText.size()), \
                            &authInfo, nullptr, 0, reinterpret_cast<PUCHAR>(plainText.data()), static_cast<ULONG>(plainText.size()), &plainTextSize, 0);

    BCryptDestroyKey(keyHandle);
    BCryptCloseAlgorithmProvider(algorithmHandle, 0);
    SecureZeroMemory(keyObject.data(), static_cast<SIZE_T>(keyObject.size()));
    if(status < 0)
    {
        SecureZeroMemory(plainText.data(), static_cast<SIZE_T>(plainText.size()));
        qWarning() << "AES-GCM decryption failed";
        return {};
    }
    plainText.resize(static_cast<qsizetype>(plainTextSize));
    return plainText;
}

QByteArray CredentialCipher::generateNonce() const
{
    QByteArray nonce(GcmNonceSize, Qt::Uninitialized);
    const NTSTATUS status = BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(nonce.data()), static_cast<ULONG>(nonce.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if(status<0)
    {
        qWarning() << "Failed to generate AES-GCM nonce";
        return {};
    }
    return nonce;
}
