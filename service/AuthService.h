#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QHash>
#include "AccountManager.h"
#include "CredentialStore.h"
#include "CredentialCache.h"
#include "../provider/IAuthProvider.h"

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(AccountManager* accountManager, CredentialStore* credentialStore, CredentialCache* credentialCache, QObject* parent=nullptr);
    Q_INVOKABLE void logout(const QString& providerId);
    Q_INVOKABLE void login(const QString& providerId, const QVariantMap& parameters);
    bool registerProvider(IAuthProvider* provider);
    void restoreSessions();
signals:
    void loginSucceeded();
    void loginFailed(const QString& message);
    void logoutSucceeded(const QString& providerId);
    void logoutFailed(const QString& message);
    void sessionRestored(ProviderType type);

private slots:
    void onLoginSucceeded(ProviderType type, const QString& userName, const QString& accessToken, const QDateTime& expiresAt);
    void onLoginFailed(ProviderType type, const QString& message);
    void onTokenValidated(ProviderType type, const QString& userName);
    void onTokenValidationFailed(ProviderType type, const QString& message);
private:
    void reauthenticate(ProviderType type, IAuthProvider* provider);

private:
    AccountManager* m_accountManager = nullptr;
    CredentialStore* m_credentialStore = nullptr;
    CredentialCache* m_credentialCache = nullptr;
    QHash<ProviderType, IAuthProvider*> m_providers;
    QHash<ProviderType, QVariantMap> m_pendingLoginParameters;

};
