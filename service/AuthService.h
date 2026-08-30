#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include "AccountManager.h"
#include "CredentialStore.h"
#include "../provider/IAuthProvider.h"

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(AccountManager* accountManager, CredentialStore* credentialStore, QObject* parent=nullptr);
    Q_INVOKABLE void logout(const QString& providerId);
    Q_INVOKABLE void login(const QString& providerId, const QVariantMap& parameters);
    bool registerProvider(IAuthProvider* provider);
signals:
    void loginSucceeded();
    void loginFailed(const QString& message);
    void logoutSucceeded(const QString& providerId);
    void logoutFailed(const QString& message);

private slots:
    void onLoginSucceeded(ProviderType type, const QString& userName, const QString& accessToken, const QDateTime& expiresAt);
    void onLoginFailed(ProviderType type, const QString& message);
private:
    AccountManager* m_accountManager = nullptr;
    CredentialStore* m_credentialStore = nullptr;
    QHash<ProviderType, IAuthProvider*> m_providers;
};
