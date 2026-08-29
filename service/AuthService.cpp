#include "AuthService.h"
#include <QDebug>
#include "../model/ProviderType.h"

AuthService::AuthService(AccountManager* accountManager, QObject* parent)
    : QObject(parent),m_accountManager(accountManager)
{

}

void AuthService::login(const QString& providerId, const QString& username, const QString& password)
{
    if(!m_accountManager)
    {
        emit loginFailed("Account manager is unavailable.");
        return;
    }
    const ProviderType providerType = providerTypeFromString(providerId);
    if(providerType == ProviderType::Unknown)
    {
        emit loginFailed("Unknown code platform.");
        return;
    }
    if(username.trimmed().isEmpty())
    {
        emit loginFailed("Username cannot be empty.");
        return;
    }
    if(password.isEmpty())
    {
        emit loginFailed("Password cannot be empty.");
        return;
    }
    if(m_accountManager->isLoggedIn(providerType))
    {
        emit loginFailed(providerDisplayName(providerType) + " is already logged in.");
        return;
    }
    // 目前还没有真正连接CodeArts
    // 暂时认为输入用户名和密码登录成功
    const bool accountAdded = m_accountManager->login(providerType, username.trimmed());
    if(!accountAdded)
    {
        emit loginFailed("Login failed.");
        return;
    }
    emit loginSucceeded();
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
    m_accountManager->logout(type);
    emit logoutSucceeded(providerId);
}
