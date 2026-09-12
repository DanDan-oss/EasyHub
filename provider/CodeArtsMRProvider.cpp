#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "CodeArtsMRProvider.h"

CodeArtsMRProvider::CodeArtsMRProvider(CredentialStore* credentialStore, AccountManager* accountManager, QObject* parent)
    : IMRProvider(parent), m_credentialStore(credentialStore), m_accountManager(accountManager)
{

}

ProviderType CodeArtsMRProvider::providerType() const
{
    return ProviderType::CodeArts;
}

void CodeArtsMRProvider::refresh(quint64 requestId)
{
    if(!m_credentialStore)
    {
        emit refreshFailed(providerType(), requestId, "Credential store is unavailable.");
        return;
    }
    if(!m_credentialStore->isAccessTokenValid(providerType()))
    {
        emit refreshFailed(providerType(), requestId, "Access token is unavailable or expired.");
        return;
    }
    const AccessCredential* credential = m_credentialStore->credential(providerType());
    if(!credential)
    {
        emit refreshFailed(providerType(), requestId, "Access credential is unavailable.");
        return;
    }
    if(credential->accessToken.isEmpty())
    {
        emit refreshFailed(providerType(), requestId, "Access token is empty.");
        return;
    }
    if(credential->region.isEmpty())
    {
        emit refreshFailed(providerType(), requestId, "region is unavailable.");
        return;
    }

    // 当前常驻同步集合由待检视、待合并、待审核、已创建中四个的opened 组成, 只用于取个人首页四个状态的mr列表
    // 当前API获取的MR信息,没有字段直接判断mr列表中的MR是否完成已检视/已审核
    RefreshContext context;
    context.requestId = requestId;
    context.pendingLists = 4;
    m_refreshContexts.insert(requestId, std::move(context));
    requestMergeRequestList(requestId, "assigned_to_me");
    requestMergeRequestList(requestId, "need_my_approve");
    requestMergeRequestList(requestId, "need_my_review");
    requestMergeRequestList(requestId, "created_by_me");
    return;
}

void CodeArtsMRProvider::loadMergeRequestDetail(const QString& repositoryId, int iid)
{
    if(!m_credentialStore)
    {
        emit mergeRequestDetailFailed(providerType(), "Credential store is unavailable.");
        return;
    }
    if(!m_credentialStore->isAccessTokenValid(providerType()))
    {
        emit mergeRequestDetailFailed(providerType(), "Access token is unavailable or expired.");
        return;
    }
    const AccessCredential* credential = m_credentialStore->credential(providerType());
    if(!credential)
    {
        emit mergeRequestDetailFailed(providerType(), "Access credential is unavailable.");
        return;
    }
    qDebug() << "CodeArts load MR detail:" << repositoryId << iid;
    // GET /v4/repositories/{repository_id}/merge-requests/{merge_request_iid}
    const QUrl url(QString("https://codehub-ext.%1.myhuaweicloud.com/v4/repositories/%2/merge-requests/%3").arg(credential->region).arg(repositoryId).arg(iid));
    qDebug() << "Loading CodeArts MR detail." << "Repository:" <<repositoryId <<"IID:" <<iid;
    QNetworkRequest request(url);
    request.setRawHeader("X-Auth-Token", credential->accessToken.toUtf8());
    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        const auto deleteLater = qScopeGuard([reply](){ reply->deleteLater();});
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "CodeArts MR detail failed:" << statusCode << reply->errorString();
            if(!body.isEmpty())
                qWarning() << "CodeArts MR detail response:" << QString::fromUtf8(body);
            emit mergeRequestDetailFailed(providerType(), reply->errorString());
            return;
        }
        handleDetailReply(body);
    });
}

void CodeArtsMRProvider::handleDetailReply(const QByteArray& body)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        emit mergeRequestDetailFailed(providerType(), "Invalid MR detail response.");
        return;
    }
    if(!document.isObject())
    {
        emit mergeRequestDetailFailed(providerType(), "Unexpected MR detail response.");
        return;
    }
    const QJsonObject object = document.object();
/*
    qDebug() << "CodeArts MR detail keys:" << object.keys();
*/
    MergeRequestDetail detail;
    detail.key.providerType = providerType();
    detail.key.repositoryId = QString::number(object.value("repository_id").toInteger());
    detail.key.iid = object.value("iid").toInt();
    detail.title = object.value("title").toString();
    detail.description = object.value("description").toString();
    detail.sourceBranch = object.value("source_branch").toString();
    detail.targetBranch = object.value("target_branch").toString();
    detail.state = object.value("state").toString();
    detail.pipelineStatus = object.value("pipeline_status").toString();
    detail.mergeStatus = object.value("merge_status").toString();
    detail.webUrl = object.value("web_url").toString();
    const QJsonObject author = object.value("author").toObject();
    detail.authorName = author.value("name").toString();
    const QJsonObject assignee = object.value("assignee").toObject();
    detail.assigneeName = assignee.value("name").toString();
    const QJsonObject targetRepository = object.value("target_repository").toObject();
    detail.projectName = targetRepository.value("name").toString();
    qDebug() << "CodeArts MR detail loaded:" << detail.key.repositoryId << detail.key.iid << detail.title;
    emit mergeRequestDetailLoaded(providerType(), detail);
}

QString CodeArtsMRProvider::repositoryIdFromListItem(const QJsonObject& object)
{
/*
    qDebug() << "========================";
    qDebug() << object.keys();
    qDebug() << "========================";
    qDebug() << "repository_id" << object.value("target_project_id").toInteger();
    qDebug() << "source_repository_id" << object.value("source_repository_id").toInteger();
    qDebug() << "target_repository_id" << object.value("target_repository_id").toInteger();
    qDebug() << "source_project_id" << object.value("source_project_id").toInteger();
    qDebug() << "target_project_id" << object.value("target_project_id").toInteger();
*/
    const qint64 targetProjectId = object.value("target_project_id").toInteger();
    if(targetProjectId > 0)
        return QString::number(targetProjectId);
    const qint64 sourceProjectId = object.value("source_project_id").toInteger();
    if(sourceProjectId > 0)
        return QString::number(sourceProjectId);
    return {};
}

void CodeArtsMRProvider::requestMergeRequestList(quint64 requestId, const QString& scope)
{
    const AccessCredential* credential = m_credentialStore->credential(providerType());
    if(!credential)
        return;
    QUrl url = QString("https://codehub-ext.%1.myhuaweicloud.com/v4/merge-requests").arg(credential->region);
    QUrlQuery query;
    query.addQueryItem("state", "opened");
    query.addQueryItem("scope", scope);
    query.addQueryItem("order_by", "updated_at");
    query.addQueryItem("sort", "desc");
    query.addQueryItem("offset", "0");
    query.addQueryItem("limit", "50");
    query.addQueryItem("view", "basic");
    url.setQuery(query);
    qDebug() << "CodeArts MR list request:" << "scope:" << scope <<url;
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json;charset=utf8");
    request.setRawHeader("X-Auth-Token", credential->accessToken.toUtf8());
    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId, scope]()
    {
        if(!reply)
            return;
        const auto deleteLater = qScopeGuard([reply](){reply->deleteLater();});
        auto contextIt = m_refreshContexts.find(requestId);
        if(contextIt == m_refreshContexts.end())
            return;
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "CodeArts MR list failed: " << "scope:" << scope << "status:" <<statusCode << reply->errorString();
            m_refreshContexts.erase(contextIt);
            emit refreshFailed(providerType(),  requestId, reply->errorString());
            return;
        }
        handleMergeRequestListReply(body, requestId, scope);

    });
}

void CodeArtsMRProvider::handleMergeRequestListReply(const QByteArray& body, quint64 requestId, const QString& scope)
{
    auto contextIt = m_refreshContexts.find(requestId);
    if(contextIt == m_refreshContexts.end())
        return;
    RefreshContext& context = contextIt.value();
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        m_refreshContexts.erase(contextIt);
        emit refreshFailed(providerType(), requestId, "Invalid MR response.");
        return;
    }
    if(!document.isArray())
    {
        m_refreshContexts.erase(contextIt);
        emit refreshFailed(providerType(), requestId, "Unexpected MR response.");
        return;
    }
    const QJsonArray items = document.array();
    qDebug() << "CodeArts MR list loaded: " << "scope:" << scope << "count:" << items.size();
    for(const QJsonValue& value : items)
    {
        if(!value.isObject())
            continue;
        const QJsonObject object = value.toObject();
        const int iid = object.value("iid").toInt();
        const QString repositoryId = repositoryIdFromListItem(object);
        if(repositoryId.isEmpty())
            continue;
        if(iid <= 0)
            continue;

        // 由另外一个待处理MR列表加入进来,同一个MR可能同时属于待合并/待审核/待检视
        // 使用repositoryId + iid 作为平台内唯一键
        // 将三个MR列表返回的结果合并成一个集合
        const QString mergeRequestKey = repositoryId + ":" + QString::number(iid);
        if(context.mergeRequestIndexes.contains(mergeRequestKey))
            continue;
        MergeRequest mergeRequest;
        mergeRequest.key.providerType = providerType();
        mergeRequest.key.repositoryId = repositoryId;
        mergeRequest.key.iid = iid;
        mergeRequest.title = object.value("title").toString();
        mergeRequest.projectName = projectNameFromListItem(object);
        mergeRequest.sourceBranch = object.value("source_branch").toString();
        mergeRequest.targetBranch = object.value("target_branch").toString();
        mergeRequest.mrState = object.value("state").toString();
        mergeRequest.pipelineStatus = object.value("pipeline_status").toString();
        mergeRequest.addedLines = object.value("added_lines").toInt();
        mergeRequest.deletedLines = object.value("removed_lines").toInt();
        mergeRequest.webUrl = object.value("web_url").toString();
        const int index = context.mergeRequests.size();
        context.mergeRequests.append(std::move(mergeRequest));
        context.mergeRequestIndexes.insert(mergeRequestKey, index);
    }
    // 三个List请求都返回后才获取完毕后
    --context.pendingLists;
    if(context.pendingLists >0)
        return;
    qDebug() << "CodeArts MR candidate union:" << context.mergeRequests.size();
    startDetailRequests(requestId);
}

void CodeArtsMRProvider::startDetailRequests(quint64 requestId)
{
    auto contextIt = m_refreshContexts.find(requestId);
    if(contextIt == m_refreshContexts.end())
        return;
    RefreshContext& context = contextIt.value();
    // List 只负责个人MR列表
    // 合并/审核/检视等真状态通过MR详情API获取
    context.pendingDetails = context.mergeRequests.size();
    if(context.pendingDetails == 0)
    {
        completeRefreshIfReady(requestId);
        return;
    }
    for(int i=0; i<context.mergeRequests.size(); ++i)
    {
        requestMergeRequestDetail(requestId, i);
    }
}

void CodeArtsMRProvider::requestMergeRequestDetail(quint64 requestId, int index)
{
    auto contextIt = m_refreshContexts.find(requestId);
    if(contextIt == m_refreshContexts.end())
        return;
    RefreshContext& context = contextIt.value();
    if(index < 0 || index >= context.mergeRequests.size())
    {
        --context.pendingDetails;
        completeRefreshIfReady(requestId);
        return;
    }
    const AccessCredential* credential  = m_credentialStore->credential(providerType());
    if(!credential)
    {
        --context.pendingDetails;
        completeRefreshIfReady(requestId);
        return;
    }
    const MergeRequest& mergeRequest = context.mergeRequests.at(index);
    QUrl url(QString("https://codehub-ext.%1.myhuaweicloud.com/v4/repositories/%2/merge-requests/%3").arg(credential->region).arg(mergeRequest.key.repositoryId).arg(mergeRequest.key.iid));

    QUrlQuery query;
    query.addQueryItem("view", "basic");
    url.setQuery(query);

    qDebug() << "CodeArts MR detail request:" << mergeRequest.key.repositoryId << mergeRequest.key.iid;

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json;charset=utf8");
    request.setRawHeader("X-Auth-Token", credential->accessToken.toUtf8());

    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId, index]()
    {
        if(!reply)
            return;
        const auto deleteLater = qScopeGuard([reply](){reply->deleteLater();});

        // 本轮refresh可能已经失败或者清理,此时晚到的网络响应直接丢弃
        auto contextIt = m_refreshContexts.find(requestId);
        if(contextIt == m_refreshContexts.end())
            return;
        RefreshContext& context = contextIt.value();
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "CodeArts MR detail refresh failed:" << statusCode << reply->errorString();
            --context.pendingDetails;
            completeRefreshIfReady(requestId);
            return;
        }

        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
        if(parseError.error != QJsonParseError::NoError || !document.isObject())
        {
            qWarning() << "Invalid CodeArts MR detail response.";
            --context.pendingDetails;
            completeRefreshIfReady(requestId);
            return;
        }
        if(index < 0 || index >= context.mergeRequests.size())
        {
            --context.pendingDetails;
            completeRefreshIfReady(requestId);
            return;
        }

        const QJsonObject object = document.object();
        MergeRequest& mergeRequest = context.mergeRequests[index];

        // /v4/repositories/{repository_id}/merge-requests/{merge_request_iid} 中的获取的MR详情字段比 /v4/merge-requests 更完整.
        // 本轮同步最终以这个Detail完整返回的数据为准.
        const qint64 repositoryId = object.value("repository_id").toInteger();
        if(repositoryId > 0)
            mergeRequest.key.repositoryId = QString::number(repositoryId);
        mergeRequest.title = object.value("title").toString(mergeRequest.title);
        mergeRequest.sourceBranch = object.value("source_branch").toString(mergeRequest.sourceBranch);
        mergeRequest.targetBranch = object.value("target_branch").toString(mergeRequest.targetBranch);
        mergeRequest.mrState = object.value("state").toString(mergeRequest.mrState);
        mergeRequest.webUrl = object.value("web_url").toString(mergeRequest.webUrl);
        const QJsonObject targetRepository = object.value("target_repository").toObject();
        const QString projectName = targetRepository.value("name").toString();
        if(!projectName.isEmpty())
            mergeRequest.projectName = projectName;
        const  QJsonObject diff = object.value("merge_request_diff").toObject();
        if(!diff.isEmpty())
        {
            mergeRequest.addedLines = diff.value("added_lines").toInt();
            mergeRequest.deletedLines = diff.value("removed_lines").toInt();
        }
        // 解析用户于MR之间的关系
        const ProviderAccount* account = nullptr;
        if(m_accountManager)
            account = m_accountManager->account(providerType());
        do
        {
            if(!account)
            {
                qWarning() << "CodeArts account is unavailable while parsing MR relations.";
                break;
            }
            if(account->remoteUserId.isEmpty())
            {
                qWarning() << "CodeArts remote user ID is empty.";
                break;
            }
            parseMergeRequestRelations(mergeRequest, object, account->remoteUserId);
            qDebug() << "CodeArts MR relations:" << mergeRequest.key.iid
                     << "assigned:" << mergeRequest.relations.assignedToMe
                     << "approve:" << mergeRequest.relations.needMyApprove
                     << "review:" << mergeRequest.relations.needMyReview
                     << "created:" << mergeRequest.relations.createdByMe;

        }while(0);

        qDebug() << "CodeArts MR detail refreshed:" << mergeRequest.key.repositoryId <<  mergeRequest.key.iid;
        --context.pendingDetails;
        completeRefreshIfReady(requestId);
    });
}

void CodeArtsMRProvider::completeRefreshIfReady(quint64 requestId)
{
    auto contextIt = m_refreshContexts.find(requestId);
    if(contextIt == m_refreshContexts.end())
        return;
    RefreshContext& context = contextIt.value();
    // List 和Detail 共同组成一次完整同步
    // 如果仍有请求进行中,不能向上层发布本轮尚未完成的数据
    if(context.pendingLists > 0)
        return;
    if(context.pendingDetails > 0)
        return;
    QList<MergeRequest> mergeRequests = std::move(context.mergeRequests);
    m_refreshContexts.erase(contextIt);
    qDebug() << providerTypeToString(providerType()) << "MR sync completed:" << mergeRequests.size();
    emit mergeRequestsLoaded(providerType(), requestId, mergeRequests);
}

QString CodeArtsMRProvider::projectNameFromListItem(const QJsonObject& object)
{
    const QJsonObject targetProject = object.value("target_project").toObject();
    const QString targetName = targetProject.value("name").toString();
    if(!targetName.isEmpty())
        return targetName;
    const QJsonObject sourceProject = object.value("source_project").toObject();
    const QString sourceName = sourceProject.value("name").toString();
    if(!sourceName.isEmpty())
        return sourceName;
    return object.value("product_name").toString();
}

bool CodeArtsMRProvider::containsUser(const QJsonArray& users, const QString& remoteUserId) const
{
    for(const QJsonValue& value :  users)
    {
        if(!value.isObject())
            continue;
        const QJsonObject user = value.toObject();
        if(user.value("username").toString() == remoteUserId)
            return true;
    }
    return false;
}

QString CodeArtsMRProvider::userState(const QJsonArray& users, const QString& remoteUserId) const
{
    for(const QJsonValue& value : users)
    {
        if(!value.isObject())
            continue;
        const QJsonObject user = value.toObject();
        if(user.value("username").toString() != remoteUserId)
            continue;
        return user.value("state").toString();
    }
    return {};
}

void CodeArtsMRProvider::parseMergeRequestRelations(MergeRequest& mergeRequest, const QJsonObject& object, const QString& remoteUserId) const
{
    if(remoteUserId.isEmpty())
        return;

    const QJsonObject author = object.value("author").toObject();
    /*
    qDebug() << "CodeArts author:"
             << "id: " << author.value("id").toInteger()
             << "username: " << author.value("username").toString()
             << "name: " << author.value("name").toString();
    qDebug() << "Current account remote user id:" << remoteUserId;
    */

    //已创建MR
    mergeRequest.relations.createdByMe = author.value("username").toString() == remoteUserId;

    // 待合并MR
    const QJsonArray assignees = object.value("merge_request_assignee_list").toArray();
    mergeRequest.relations.assignedToMe = containsUser(assignees, remoteUserId);

    // 待审核MR
    const QJsonArray approvers = object.value("approval_merge_request_approvers").toArray();
    const QString approveState = userState(approvers, remoteUserId);
    mergeRequest.relations.needMyApprove = !approveState.isEmpty() && approveState != "approve";

    // 待检视
    const QJsonArray reviewers = object.value("approval_merge_request_reviewers").toArray();
    const QString reviewState = userState(reviewers, remoteUserId);
    mergeRequest.relations.needMyReview = !reviewState.isEmpty()  && reviewState != "true";
    /*
    qDebug() << "CodeArts assignees:" << QJsonDocument(assignees).toJson(QJsonDocument::Compact);
    qDebug() << "CodeArts approvers:" << QJsonDocument(approvers).toJson(QJsonDocument::Compact);
    qDebug() << "CodeArts reviewers:" << QJsonDocument(reviewers).toJson(QJsonDocument::Compact);
    */
}
