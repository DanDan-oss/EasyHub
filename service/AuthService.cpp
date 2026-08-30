#include "AuthService.h"
#include <QDebug>
#include "../model/ProviderType.h"
#include "../provider/CodeArtsAuthProvider.h"

AuthService::AuthService(AccountManager* accountManager, CredentialStore* credentialStore, QObject* parent)
    : QObject(parent),m_accountManager(accountManager),m_credentialStore(credentialStore)
{
}

void AuthService::login(const QString& providerId, const QVariantMap& parameters)
{
    if(!m_accountManager)
    {
        emit loginFailed("Account manager is unavailable.");
        return;
    }
    const ProviderType type = providerTypeFromString(providerId);
    if(type == ProviderType::Unknown)
    {
        emit loginFailed("Unknown code platform");
        return;
    }
    if(m_accountManager->isLoggedIn(type))
    {
        emit loginFailed(providerDisplayName(type) + " is already logged in.");
        return;
    }
    auto it = m_providers.constFind(type);
    if(it == m_providers.constEnd())
    {
        emit loginFailed("Authentication provider is unavailable.");
        return;
    }
    it.value()->login(parameters);
}

bool AuthService::registerProvider(IAuthProvider* provider)
{
    if(!provider)
        return false;
    const ProviderType type = provider->providerType();
    if(type== ProviderType::Unknown)
    {
        emit loginFailed("register proveider type is Unknown.");
        return false;
    }
    if(m_providers.contains(type))
    {
        emit loginFailed("register proveider type already exists.");
        return false;
    }

    m_providers.insert(type, provider);
    connect(provider, &IAuthProvider::loginSucceeded, this, &AuthService::onLoginSucceeded);
    connect(provider, &IAuthProvider::loginFailed, this, &AuthService::onLoginFailed);
    return true;
}

void AuthService::logout(const QString& providerId)
{
    if(!m_accountManager)
    {
        emit logoutFailed("Account manager is unavailable.");
        return;
    }
    const ProviderType type = providerTypeFromString(providerId);
    if(type == ProviderType::Unknown)
    {
        emit logoutFailed("Unkmown code platform.");
        return;
    }
    if(!m_accountManager->isLoggedIn(type))
    {
        emit logoutFailed(providerDisplayName(type) + " is not logged in.");
        return;
    }
    m_accountManager->removeAccount(type);
    emit logoutSucceeded(providerId);
}

void AuthService::onLoginSucceeded(ProviderType type, const QString& userName, const QString& accessToken, const QDateTime& expiresAt)
{
    if(!m_accountManager)
    {
        emit loginFailed("Account manager is unavailable.");
        return;
    }
    if(!m_credentialStore)
    {
        emit loginFailed("Credential store is unavailable.");
        return;
    }
    m_credentialStore->setAccessToken(type, accessToken, expiresAt);
    if(!m_accountManager->addAccount(type,userName))
    {
        m_credentialStore->clear(type);
        emit loginFailed("Failed to add account");
        return;
    }
    emit loginSucceeded();
}

void AuthService::onLoginFailed(ProviderType type, const QString& message)
{
    Q_UNUSED(type);
    emit loginFailed(message);
}
