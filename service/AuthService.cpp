#include "AuthService.h"
#include <QDebug>
#include "../model/ProviderType.h"

AuthService::AuthService(AccountManager* accountManager, CredentialStore* credentialStore, CredentialCache* credentialCache, QObject* parent)
    : QObject(parent),m_accountManager(accountManager),m_credentialStore(credentialStore), m_credentialCache(credentialCache)
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
    m_pendingLoginParameters.insert(type, parameters);
    it.value()->login(parameters);
}

bool AuthService::registerProvider(IAuthProvider* provider)
{
    if(!provider)
        return false;
    const ProviderType type = provider->providerType();
    if(type== ProviderType::Unknown)
    {
        qWarning() << "Register provider type is Unknown.";
        return false;
    }
    if(m_providers.contains(type))
    {
        qWarning() << "Proveider already registered: " << providerTypeToString(type);
        return false;
    }

    m_providers.insert(type, provider);
    connect(provider, &IAuthProvider::loginSucceeded, this, &AuthService::onLoginSucceeded);
    connect(provider, &IAuthProvider::loginFailed, this, &AuthService::onLoginFailed);
    connect(provider, &IAuthProvider::tokenValidated, this, &AuthService::onTokenValidated);
    connect(provider, &IAuthProvider::tokenValidationFailed, this, &AuthService::onTokenValidationFailed);
    return true;
}

void AuthService::restoreSessions()
{
    if(!m_credentialStore || !m_credentialCache)
        return;
    for(auto it = m_providers.constBegin(); it != m_providers.constEnd(); ++it)
    {
        const ProviderType type = it.key();
        IAuthProvider* provider = it.value();
        if(!provider)
            continue;
        qDebug() << "Restoring session for" << providerTypeToString(type);
        const auto accessCredential = m_credentialCache ->loadAccessCredential(type);
        if(!accessCredential)
        {
            qWarning() << "Access credential not found for " << providerTypeToString(type);
            reauthenticate(type, provider);
            continue;
        }
        qDebug() << "Access credential found: " << accessCredential.has_value();
        m_credentialStore->setCredential(type, *accessCredential);
        if(!m_credentialStore->isAccessTokenValid(type))
        {
            m_credentialStore->clear(type);
            m_credentialCache->clearAccessCredential(type);
            reauthenticate(type, provider);
            continue;
        }
        qDebug() << "Access token locally valid for " << providerTypeToString(type);
        qDebug() << "Validating token for " << providerTypeToString(type);
        provider->validateToken(accessCredential->accessToken);

    }
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
        emit logoutFailed("Unknown code platform.");
        return;
    }
    if(!m_accountManager->isLoggedIn(type))
    {
        emit logoutFailed(providerDisplayName(type) + " is not logged in.");
        return;
    }
    if(!m_accountManager->removeAccount(type))
    {
        emit logoutFailed("Failed to remove account.");
        return;
    }
    if(m_credentialStore)
        m_credentialStore->clear(type);
    if(m_credentialCache)
    {
        const bool loginCleared = m_credentialCache->clearLoginParameters(type);
        const bool tokenCleared = m_credentialCache->clearAccessCredential(type);
        if(!loginCleared || !tokenCleared)
            qWarning() << "Failed to fully clear cached credentials for" << providerTypeToString(type);
    }
    emit logoutSucceeded(providerId);
}

void AuthService::onLoginSucceeded(ProviderType type, const QString& userName, const QString& remoteUserId, const AccessCredential& credential)
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
    if(!m_credentialCache)
    {
        emit loginFailed("Credential cache is unavailable.");
        return;
    }

    m_credentialStore->setCredential(type, credential);
    const QVariantMap loginParameters = m_pendingLoginParameters.take(type);
    if(!loginParameters.isEmpty())
        if(!m_credentialCache->saveLoginParameters(type, loginParameters))
            qWarning() << "Failed to cache login parameters for " <<providerTypeToString(type);
    if(!m_credentialCache->saveAccessCredential(type, credential))
        qWarning() << "Failed to cache access credential for " << providerTypeToString(type);

    if(!m_accountManager->addAccount(type, userName, remoteUserId))
    {
        m_credentialStore->clear(type);
        emit loginFailed("Failed to add account.");
        return;
    }
    emit loginSucceeded();
}

void AuthService::onLoginFailed(ProviderType type, const QString& message)
{
    m_pendingLoginParameters.remove(type);
    emit loginFailed(message);
}

void AuthService::onTokenValidated(ProviderType type, const QString& userName, const QString& remoteUserId)
{
    if(!m_accountManager)
        return;
    if(!m_accountManager->isLoggedIn(type))
        if(!m_accountManager->addAccount(type, userName, remoteUserId))
            return;
    qDebug() << "Token validated for " << providerTypeToString(type);
    emit sessionRestored(type);
}

void AuthService::onTokenValidationFailed(ProviderType type, const QString& message)
{
    Q_UNUSED(message);
    if(m_credentialStore)
        m_credentialStore->clear(type);
    if(m_credentialCache)
        m_credentialCache->clearAccessCredential(type);
    qDebug() << "Token validation failed for " << providerTypeToString(type);
    IAuthProvider* provider = m_providers.value(type, nullptr);
    if(!provider)
        return;
    reauthenticate(type, provider);
}

void AuthService::reauthenticate(ProviderType type, IAuthProvider* provider)
{
    if(!provider || !m_credentialCache)
        return;
    qDebug() << "Try to load login parameters for" << providerTypeToString(type);
    const auto loginParameters = m_credentialCache->loadLoginParameters(type);
    if(!loginParameters)
    {
        qDebug() << "Login parameters not found for " << providerTypeToString(type);
        return;
    }
    qDebug() << "Reauthenticating" << providerTypeToString(type);
    m_pendingLoginParameters.insert(type, *loginParameters);
    provider->login(*loginParameters);
}
