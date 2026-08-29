#include "ProviderListModel.h"

ProviderListModel::ProviderListModel(AccountManager* accountManager, QObject* parent)
    : QAbstractListModel(parent), m_accountManager(accountManager)
{
    if(!m_accountManager)
        return;
    connect(m_accountManager, &AccountManager::providerLoggedIn, this, &ProviderListModel::onProviderLoginStateChanged);
    connect(m_accountManager, &AccountManager::providerLoggedOut, this, &ProviderListModel::onProviderLoginStateChanged);
    connect(m_accountManager, &AccountManager::currentProviderChanged, this, [this](ProviderType){
        emit currentProviderChanged();
    });
    return;
}

int ProviderListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return static_cast<int>(kProviderDescriptors.size());
}

QVariant ProviderListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};
    if(index.row() <0 || index.row() >= static_cast<int>(kProviderDescriptors.size()))
        return {};
    const ProviderDescriptor& provider = kProviderDescriptors.at(index.row());
    switch (role) {
    case ProviderTypeRole:
        return static_cast<int>(provider.type);
    case ProviderIdRole:
        return QString::fromUtf8(provider.id);
    case ProviderNameRole:
        return QString::fromUtf8(provider.displayName);
    case IconSourceRole:
        return QString::fromUtf8(provider.iconSource);
    case LoggedInRole:
        return m_accountManager ? m_accountManager->isLoggedIn(provider.type) : false;
    default:
        return {};
    }
}

QHash<int, QByteArray> ProviderListModel::roleNames() const
{
    return {
        {ProviderTypeRole, "providerType" },
        {ProviderIdRole, "providerId" },
        {ProviderNameRole, "providerName" },
        {IconSourceRole, "iconSource"},
        {LoggedInRole, "loggedIn"}
    };
}


QString ProviderListModel::currentProviderId() const
{
    if(!m_accountManager)
        return {};
    const ProviderType type = m_accountManager->currentProvider();
    if(type == ProviderType::Unknown)
        return {};
    return providerTypeToString(m_accountManager->currentProvider());
}

QString ProviderListModel::currentProviderName() const
{
    if(!m_accountManager)
        return {};
    const ProviderType type = m_accountManager->currentProvider();
    if(type == ProviderType::Unknown)
        return {};
    return providerDisplayName(type);
}

bool ProviderListModel::setCurrentProvider(const QString& providerId)
{
    if(!m_accountManager)
        return false;
    const ProviderType type = providerTypeFromString(providerId);
    if(type == ProviderType::Unknown)
        return false;
    return m_accountManager->setCurrentProvider(type);
}

int ProviderListModel::rowForProvider(ProviderType type) const
{
    for(int i =  0; i < static_cast<int>(kProviderDescriptors.size()); ++i)
        if(kProviderDescriptors.at(i).type == type)
            return i;
    return -1;
}

void ProviderListModel::onProviderLoginStateChanged(ProviderType type)
{
    const int row = rowForProvider(type);
    if(row < 0)
        return;
    const QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, {LoggedInRole});
}

QString ProviderListModel::providerIdAt(int row) const
{
    if(row < 0 || row >= static_cast<int>(kProviderDescriptors.size()))
        return {};
    return QString::fromUtf8(kProviderDescriptors.at(row).id);
}

QString ProviderListModel::providerNameAt(int row) const
{
    if(row < 0 || row >= static_cast<int>(kProviderDescriptors.size()))
        return {};
    return QString::fromUtf8(kProviderDescriptors.at(row).displayName);
}

QString ProviderListModel::iconSourceAt(int row) const
{
    if(row < 0 || row >= static_cast<int>(kProviderDescriptors.size()))
        return {};
    return QString::fromUtf8(kProviderDescriptors.at(row).iconSource);
}

bool ProviderListModel::loggedInAt(int row) const
{
    if(row < 0 || row >= static_cast<int>(kProviderDescriptors.size()))
        return false;
    if(!m_accountManager)
        return false;
    return m_accountManager->isLoggedIn(kProviderDescriptors.at(row).type);
}

