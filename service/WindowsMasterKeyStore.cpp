#include <QDebug>
#include <Windows.h>
#include <bcrypt.h>
#include <wincred.h>
#include "WindowsMasterKeyStore.h"

namespace {
constexpr qsizetype MasterKeySize = 32;
constexpr wchar_t MasterKeyTarget[] = L"EasyHub/MasterKey";
}

std::optional<QByteArray> WindowsMasterKeyStore::load() const
{
    PCREDENTIALW credential = nullptr;
    if(!CredReadW(MasterKeyTarget, CRED_TYPE_GENERIC, 0, &credential))
        return {};
    if(!credential)
        return {};
    QByteArray key(reinterpret_cast<const char*>(credential->CredentialBlob), static_cast<qsizetype>(credential->CredentialBlobSize));
    CredFree(credential);
    if(key.size() != MasterKeySize)
    {
        clear(key);
        return {};
    }
    return key;
}

std::optional<QByteArray> WindowsMasterKeyStore::loadOrCreate()
{
    if(auto key = load())
        return key;
    QByteArray key = generate();
    if(key.size() != MasterKeySize)
        return {};
    if(!save(key))
    {
        clear(key);
        return {};
    }
    return key;
}

void WindowsMasterKeyStore::clear(QByteArray& key) const
{
    if(key.isEmpty())
        return;
    SecureZeroMemory(key.data(), static_cast<SIZE_T>(key.size()));
    key.clear();
}

QByteArray  WindowsMasterKeyStore::generate() const
{
    QByteArray key(MasterKeySize, Qt::Uninitialized);
    CONST NTSTATUS status = BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(key.data()), static_cast<ULONG>(key.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if(status<0)
    {
        clear(key);
        qWarning() <<"Failed to generate master key";
        return {};
    }
    return key;
}

bool WindowsMasterKeyStore::save(const QByteArray& key) const
{
    if(key.size() != MasterKeySize)
        return false;
    CREDENTIALW credential {};
    credential.Type = CRED_TYPE_GENERIC;
    credential.TargetName = const_cast<LPWSTR>(MasterKeyTarget);
    credential.CredentialBlobSize = static_cast<DWORD>(key.size());
    credential.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<char*>(key.constData()));
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    if(!CredWriteW(&credential, 0))
    {
        qWarning() <<"Failed to save master key: " <<GetLastError();
        return false;
    }
    return true;
}
