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
    connect(provider, &IMRProvider::mergeRequestDetailLoaded, this, &MRService::onMergeRequestDetailLoaded);
    connect(provider, &IMRProvider::mergeRequestDetailFailed, this, &MRService::onMergeRequestDetailFailed);
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
    ++m_refreshRequestId;
    provider->refresh(m_refreshRequestId);
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
    m_model.clear();
    qDebug() << "Refreshing MR state: " << stats;
    refresh();
}

void MRService::onCurrentProviderChanged(ProviderType type)
{
    qDebug() << "MR current provider changed." << providerTypeToString(type);
    if(m_mergeRequests.contains(type))
    {
        updateModel();
        return;
    }
    refresh();
}

void MRService::onMergeRequestsLoaded(ProviderType type, quint64 requestId, const QList<MergeRequest>& mergeRequests)
{
    if(!m_accountManager)
        return;

    // 已经切换到别的平台的话,这个异步结果直接丢弃
    if(type != m_accountManager->currentProvider())
        return;

    // 过时的请求结果直接丢弃
    if(requestId != m_refreshRequestId)
    {
        qDebug() << "Discard stale MR response:" << requestId << " current:" << m_refreshRequestId;
        return;
    }
    // 保存本次完整的同步结果
    m_mergeRequests.insert(type, mergeRequests);
    qDebug() << providerTypeToString(type) << "MR cache updated:" << mergeRequests.size();
    updateModel();
}

void MRService::onRefreshFailed(ProviderType type, quint64 requestId, const QString& message)
{
    if(!m_accountManager)
        return;
    if(type != m_accountManager->currentProvider())
        return;
    if(requestId != m_refreshRequestId)
    {
        qWarning() << "Discard stale MR refresh failed:" <<requestId << "current:" << m_refreshRequestId;
        return;
    }
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
    const auto it = m_mergeRequests.constFind(type);
    if(it == m_mergeRequests.constEnd())
        return;
    const QList<MergeRequest>& mergeRequests = it.value();
    qDebug() << "iipdate MR model:"
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

