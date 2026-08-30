#include <QDebug>
#include "MRService.h"

MRService::MRService(AccountManager* accountManager, QObject* parent)
    : QObject(parent), m_accountManager(accountManager)
{
    if(!m_accountManager)
        return;
    connect(m_accountManager, &AccountManager::currentProviderChanged, this, &MRService::onCurrentProviderChanged);
}

MRListModel* MRService::model()
{
    return &this->m_model;
}

void MRService::registerProvider(IMRProvider* provider)
{
    if(!provider)
        return;
    const ProviderType type = provider->providerType();
    if(type == ProviderType::Unknown)
        return;
    if(m_providers.contains(type))
        return;

    m_providers.insert(type, provider);
    connect(provider, &IMRProvider::mergeRequestsLoaded, this, &MRService::onMergeRequestsLoaded);
    connect(provider, &IMRProvider::refreshFailed, this, &MRService::onRefreshFailed);

}

void MRService::refresh()
{
    if(!m_accountManager)
        return;
    const ProviderType type = m_accountManager->currentProvider();
    if(type == ProviderType::Unknown)
    {
        m_model.clear();
        return;
    }

    IMRProvider* provider = currentProvider();
    if(!provider)
    {
        qWarning() << "No MR provider registered for: " <<providerTypeToString(type);
        return;
    }
    provider->refresh();
}

void MRService::onCurrentProviderChanged(ProviderType type)
{
    qDebug() << "MR current provider changed." << providerTypeToString(type);
    refresh();
}

void MRService::onMergeRequestsLoaded(ProviderType type, const QList<MergeRequest>& mergeRequests)
{
    if(!m_accountManager)
        return;

    // 已经切换到别的平台的话,这个异步结果直接丢弃
    if(type != m_accountManager->currentProvider())
        return;

    m_model.clear();
    for(const MergeRequest& mr : mergeRequests)
        m_model.addMergeRequest(mr);
}

void MRService::onRefreshFailed(ProviderType type, const QString& message)
{
    if(!m_accountManager)
        return;
    if(type != m_accountManager->currentProvider())
        return;
    qWarning() << "MR refresh failed:" << providerTypeToString(type) << message;
    // emit refrenshFailed(message)
}

IMRProvider* MRService::currentProvider() const
{
    if(!m_accountManager)
        return nullptr;
    const ProviderType type = m_accountManager->currentProvider();
    auto it = m_providers.constFind(type);
    if(it == m_providers.constEnd())
        return nullptr;
    return it.value();
}

