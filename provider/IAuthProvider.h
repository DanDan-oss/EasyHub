#pragma once
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

#include "../model/ProviderType.h"
#include "../service/CredentialStore.h"


/*
struct GitLabLoginCredential
{
    QString userName;
    QString password;
};
*/

class IAuthProvider : public QObject
{
    Q_OBJECT
public:
    explicit IAuthProvider(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    ~IAuthProvider() override = default;
    virtual ProviderType providerType() const = 0;
    virtual void login(const QVariantMap& parameters) = 0;
    virtual void validateToken(const QString& accessToken) = 0;

signals:
    void loginSucceeded(ProviderType type, const QString& username, const QString& remoteUserId, const AccessCredential& credential);
    void loginFailed(ProviderType type, const QString& message);
    void tokenValidated(ProviderType type, const QString& username, const QString& remoteUserId);
    void tokenValidationFailed(ProviderType type, const QString& message);
};
