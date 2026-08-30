#include <QDebug>
#include "AccountManager.h"

AccountManager::AccountManager(QObject* parent)
    :QObject(parent)
{

}

bool AccountManager::isLoggedIn(ProviderType type) const
{
    return m_accounts.contains(type);
}

const ProviderAccount* AccountManager::account(ProviderType type) const
{
    auto it = m_accounts.constFind(type);
    if(it == m_accounts.constEnd())
        return nullptr;
    return &it.value();
}

bool AccountManager::addAccount(ProviderType type, const QString& username)
{
    if(type == ProviderType::Unknown)
        return false;
    if(isLoggedIn(type))
        return false;
    ProviderAccount account;
    account.type = type;
    account.username = username;

    m_accounts.insert(type, account);
    emit providerLoggedIn(type);
    setCurrentProvider(type);
    qDebug() << "Provider logged in:" << providerTypeToString(type) << username;
    return true;
}

bool AccountManager::removeAccount(ProviderType type)
{
    qDebug() << "logout called:" << providerTypeToString(type);
    auto it = m_accounts.find(type);
    if(it == m_accounts.end())
        return false;
    const bool wasCurrent = (m_currentProvider == type);
    m_accounts.erase(it);
    if(wasCurrent)
    {
        // 找一个仍然已登录的平台作为当前平台
        m_currentProvider = ProviderType::Unknown;
        for(const ProviderDescriptor& provider : kProviderDescriptors)
        {
            if(isLoggedIn(provider.type))
            {
                m_currentProvider = provider.type;
                break;
            }
        }
    }

    emit providerLoggedOut(type);
    if(wasCurrent)
        emit currentProviderChanged(m_currentProvider);
    return true;
}

ProviderType AccountManager::currentProvider() const
{
    return m_currentProvider;
}

bool AccountManager::setCurrentProvider(ProviderType type)
{
    if(!isLoggedIn(type))
        return false;
    if(m_currentProvider == type)
        return true;
    m_currentProvider = type;
    emit currentProviderChanged(type);
    return true;
}
