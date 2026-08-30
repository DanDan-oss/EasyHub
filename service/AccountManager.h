#pragma once

#include <QObject>
#include <QHash>

#include "../model/ProviderAccount.h"

class AccountManager : public QObject
{
    Q_OBJECT
public:
    explicit AccountManager(QObject* parent = nullptr);
    bool isLoggedIn(ProviderType type) const;
    const ProviderAccount* account(ProviderType type) const;
    bool addAccount(ProviderType type, const QString& username);
    bool removeAccount(ProviderType type);
    ProviderType currentProvider() const;
    bool setCurrentProvider(ProviderType type);

signals:
    void providerLoggedIn(ProviderType type);
    void providerLoggedOut(ProviderType type);
    void currentProviderChanged(ProviderType type);

private:
    QHash<ProviderType, ProviderAccount> m_accounts;
    ProviderType m_currentProvider = ProviderType::Unknown;
};

