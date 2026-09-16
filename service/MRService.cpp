#include <QDebug>
#include "MRService.h"

MRService::MRService(AccountManager* accountManager, QObject* parent)
    : QObject(parent), m_accountManager(accountManager)
{
    if(!m_accountManager)
        return;
    connect(m_accountManager, &AccountManager::currentProviderChanged, this, &MRService::onCurrentProviderChanged);
    connect(m_accountManager, &AccountManager::providerLoggedIn, this, &MRService::onProviderLoggedIn);
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
    connect(provider, &IMRProvider::mergeRequestDetailLoaded, this, &MRService::onMergeRequestDetailLoaded);
    connect(provider, &IMRProvider::mergeRequestDetailFailed, this, &MRService::onMergeRequestDetailFailed);
    connect(provider, &IMRProvider::queriedMergeRequestsLoaded, this, &MRService::onQueriedMergeRequestsLoaded);
    connect(provider, &IMRProvider::queryMergeRequestsFailed, this, &MRService::onQueryMergeRequestsFailed);
}

void MRService::loadMergeRequestDetail(const QString& repositoryId, int iid)
{
    qDebug() << "MRService load detail:" << repositoryId << iid;
    IMRProvider* provider = currentProvider();
    if(!provider)
    {
        qWarning() << "Current MR provider is unavailable.";
        return;
    }
    provider->loadMergeRequestDetail(repositoryId, iid);
}

void MRService::refresh()
{
    if(!m_accountManager)
        return;
    for(auto it = m_providers.begin(); it != m_providers.constEnd(); ++it)
    {
        const ProviderType type = it.key();
        IMRProvider* provider = it.value();
        if(!provider)
        {
            qWarning() << "No MR provider registered for: " <<providerTypeToString(type);
            continue;
        }
        if(!m_accountManager->isLoggedIn(type))
            continue;
        const quint64 requestId = ++m_requestId;
        m_refreshRequestIds.insert(type, requestId);
        qDebug() << "MR refresh started:" << providerTypeToString(type) << "requestId:" <<requestId;
        provider->refresh(requestId);
    }
}

void MRService::setCategory(int category)
{
    const auto newCategory = static_cast<MergeRequestCategory>(category);
    if(m_query.category == newCategory)
        return;
    m_query.category = newCategory;
    qDebug() << "MR category changed: " << category;
    updateModel();
}

void MRService::setState(int stats)
{
    const auto newState = static_cast<MergeRequestState>(stats);
    if(m_query.state  == newState)
        return;
    m_query.state = newState;
    qDebug() << "MR state changed: " << stats;
    if(newState == MergeRequestState::Opened)
    {
        m_queryMergeRequests.clear();
        updateModel();
        return;
    }
    IMRProvider* provider = currentProvider();
    if(!provider)
        return;
    const quint64 requestId = ++m_requestId;
    m_queryRequestId = requestId;
    m_model.clear();
    provider->loadMergeRequests(requestId, newState);
}

void MRService::onCurrentProviderChanged(ProviderType type)
{
    qDebug() << "MR current provider changed." << providerTypeToString(type);
    m_queryMergeRequests.clear();
    if(m_query.state  == MergeRequestState::Opened)
    {
        updateModel();
        return;
    }
    /*
    if(m_mergeRequests.contains(type))
    {
        updateModel();
        return;
    }
    refresh();
    */
    IMRProvider* provider = currentProvider();
    if(!provider)
    {
        m_model.clear();
        return;
    }
    const quint64 requestId =  ++m_requestId;
    m_queryRequestId = requestId;
    m_model.clear();
    provider->loadMergeRequests(requestId, m_query.state);
}

void MRService::onMergeRequestsLoaded(ProviderType type, quint64 requestId, const QList<MergeRequest>& mergeRequests)
{

    // 过时的请求结果直接丢弃
    if(requestId != m_refreshRequestIds.value(type))
    {
        qDebug() << "Discard stale MR response:" << providerTypeToString(type) << requestId << " current:" << m_refreshRequestIds.value(type);
        return;
    }
    // 保存本次完整的同步结果
    m_mergeRequests.insert(type, mergeRequests);
    qDebug() << providerTypeToString(type) << "MR cache updated:" << mergeRequests.size();

    if(!m_accountManager)
        return;

    // 如果不是当前显示的平台,不需要刷新前台
    if(type != m_accountManager->currentProvider())
        return;

    updateModel();
}

void MRService::onRefreshFailed(ProviderType type, quint64 requestId, const QString& message)
{
    if(requestId != m_refreshRequestIds.value(type))
        return;
    qWarning() << "MR refresh failed:" << providerTypeToString(type) << message;
    // emit refrenshFailed(message)
}

void MRService::onMergeRequestDetailLoaded(ProviderType type, const MergeRequestDetail& detail)
{
    if(!m_accountManager)
        return;
    if(type != m_accountManager->currentProvider())
        return;
    emit  mergeRequestDetailLoaded(detail.key.iid, detail.title, detail.projectName, detail.sourceBranch, detail.targetBranch, \
                                    detail.state, detail.pipelineStatus, detail.webUrl);
}

void MRService::onMergeRequestDetailFailed(ProviderType type, const QString& message)
{
    if(!m_accountManager)
        return;
    if(type != m_accountManager->currentProvider())
        return;
    qWarning() <<"MR detail failed:" << providerTypeToString(type) << message;
}

void MRService::onQueriedMergeRequestsLoaded(ProviderType type, quint64 requestId, MergeRequestState state, const QList<MergeRequest>& mergeRequests)
{
    if(requestId != m_queryRequestId)
        return;
    if(state != m_query.state)
        return;
    if(!m_accountManager)
        return;
    if(type != m_accountManager->currentProvider())
        return;
    IMRProvider* provider = currentProvider();
    if(!provider)
        return;
    if(type != provider->providerType())
        return;
    m_queryMergeRequests = mergeRequests;
    m_queryState = state;
    qDebug() << providerTypeToString(type)
             << "MR query cache updated:"
             << "state:" << static_cast<int>(state)
             << "count:" <<mergeRequests.size();
    updateModel();
}

void MRService::onQueryMergeRequestsFailed(ProviderType type, quint64 requestId, MergeRequestState state, const QString& error)
{
    if(requestId != m_queryRequestId)
        return;
    if(state != m_query.state)
        return;
    IMRProvider* provider = currentProvider();
    if(!provider)
        return;
    if(type != provider->providerType())
        return;
    qWarning() << providerTypeToString(type)
               << "MR query failed:"
               << "state: " <<static_cast<int>(state)
               << "error: " <<error;
    // 后面需要给QML展示错误时,再在这里发Service层信号
}

 void MRService::onProviderLoggedIn(ProviderType type)
{
     auto it = m_providers.constFind(type);
    if(it == m_providers.constEnd())
        return;
    IMRProvider* provider = it.value();
    if(!provider)
        return;
    const quint64 requestId = ++m_requestId;
    m_refreshRequestIds.insert(type, requestId);
    qDebug() << "MR initial sync started:" << providerTypeToString(type) << "requestId:" <<requestId;
    provider->refresh(requestId);
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

void MRService::updateModel()
{
    m_model.clear();
    if(!m_accountManager)
        return;
    const ProviderType type = m_accountManager->currentProvider();
    if(type == ProviderType::Unknown)
        return;
    if(m_query.state != MergeRequestState::Opened)
    {
        for(const MergeRequest& mergeRequest : m_queryMergeRequests)
        {
            if(!matchesCategory(mergeRequest))
                continue;
            m_model.addMergeRequest(mergeRequest);
        }
        return;
    }
    const auto it = m_mergeRequests.constFind(type);
    if(it == m_mergeRequests.constEnd())
        return;
    const QList<MergeRequest>& mergeRequests = it.value();
    qDebug() << "update MR model:"
             << "category:" <<static_cast<int>(m_query.category)
             << "cache:" << mergeRequests.size();

    for(const MergeRequest& mergeRequest : mergeRequests  )
    {
        const bool matched = matchesCategory(mergeRequest);
        qDebug() << "MR: " << mergeRequest.key.iid
                 << "assigned: " << mergeRequest.relations.assignedToMe
                 << "approve: " << mergeRequest.relations.needMyApprove
                 << "review: " << mergeRequest.relations.needMyReview
                 << "created: " << mergeRequest.relations.createdByMe;
        if(!matched)
            continue;
        m_model.addMergeRequest(mergeRequest);
    }
}

bool MRService::matchesCategory(const MergeRequest& mergeRequest) const
{
    switch (m_query.category) {
    case MergeRequestCategory::ToMerge:
        return mergeRequest.relations.assignedToMe;
    case MergeRequestCategory::ToApprove:
        return mergeRequest.relations.needMyApprove;
    case MergeRequestCategory::ToReview:
        return mergeRequest.relations.needMyReview;
    case MergeRequestCategory::Created:
        return mergeRequest.relations.createdByMe;
    default:
        return false;
    }
    return false;
}

