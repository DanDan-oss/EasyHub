#pragma once

#include <QObject>
#include <QString>
#include "AccountManager.h"

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(AccountManager* accountManager, QObject* parent=nullptr);
    Q_INVOKABLE void login(const QString& providerId, const QString& username, const QString& password);
    Q_INVOKABLE void logout(const QString& providerId);

signals:
    void loginSucceeded();
    void loginFailed(const QString& message);
    void logoutSucceeded(const QString& providerId);
    void logoutFailed(const QString& message);
private:
    AccountManager* m_accountManager = nullptr;
};
