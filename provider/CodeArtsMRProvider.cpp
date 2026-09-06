#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "CodeArtsMRProvider.h"

CodeArtsMRProvider::CodeArtsMRProvider(CredentialStore* credentialStore, QObject* parent)
    : IMRProvider(parent), m_credentialStore(credentialStore)
{

}

ProviderType CodeArtsMRProvider::providerType() const
{
    return ProviderType::CodeArts;
}

void CodeArtsMRProvider::refresh(const MergeRequestQuery& requestQuery, quint64 requestId)
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

    // 设置消息头
    QUrl url = QString("https://codehub-ext.%1.myhuaweicloud.com/v4/merge-requests").arg(credential->region);
    QUrlQuery query;
    query.addQueryItem("state", stateFromQuery(requestQuery.state));
    query.addQueryItem("scope", scopeFromCategory(requestQuery.category));
    query.addQueryItem("order_by", "updated_at");
    query.addQueryItem("sort", "desc");
    query.addQueryItem("offset", "0");
    query.addQueryItem("limit", "50");
    query.addQueryItem("view", "basic");
    url.setQuery(query);
    qDebug() << "Refreshing CodeArts merge requests."  << "Region:" << credential->region;
    qDebug() << "CodeArts MR request. " << url;
    qDebug() << "CodeArts MR category: " << scopeFromCategory(requestQuery.category);
    qDebug() << "CodeArts MR state: " << stateFromQuery(requestQuery.state);


    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json;charset=utf8");
    request.setRawHeader("X-Auth-Token", credential->accessToken.toUtf8());

    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId]()
    {
        if(!reply)
            return;
        const auto deleteLater = qScopeGuard([reply](){reply->deleteLater();});
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "CodeArts MR refresh failed: " << statusCode << reply->errorString();
            if(!body.isEmpty())
                qWarning() << "CodeArts error response:" << QString::fromUtf8(body);
            emit refreshFailed(providerType(), requestId, reply->errorString());
            return;
        }
        handleRefreshReply(body, requestId, statusCode);
    });

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

void CodeArtsMRProvider::handleRefreshReply(const QByteArray& body, quint64 requestId, int statusCode)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        emit refreshFailed(providerType(), requestId, "Invalid MR response.");
        return;
    }
    if(!document.isArray())
    {
        emit refreshFailed(providerType(), requestId, "Unexpected MR response.");
        return;
    }
    QList<MergeRequest> mergeRequests;
    const QJsonArray items = document.array();
    mergeRequests.reserve(items.size());
    for(const QJsonValue& value : items)
    {
        if(!value.isObject())
            continue;
        const QJsonObject object = value.toObject();
        MergeRequest mergeRequest;
        mergeRequest.key.providerType = providerType();
        mergeRequest.key.repositoryId = repositoryIdFromListItem(object);
        mergeRequest.key.iid = object.value("iid").toInt();
        mergeRequest.title = object.value("title").toString();
        mergeRequest.projectName = object.value("product_name").toString();

        mergeRequest.sourceBranch = object.value("source_branch").toString();
        mergeRequest.targetBranch = object.value("target_branch").toString();

        mergeRequest.mrState = object.value("state").toString();
        mergeRequest.pipelineStatus = object.value("pipeline_status").toString();
        mergeRequest.addedLines = object.value("added_lines").toInt();
        mergeRequest.deletedLines = object.value("removed_lines").toInt();
        mergeRequest.webUrl = object.value("web_url").toString();
        mergeRequests.append(std::move(mergeRequest));
    }
    qDebug() << providerTypeToString( providerType()) << " MR Loaded:" << mergeRequests.size();
    emit mergeRequestsLoaded(providerType(), requestId, mergeRequests);
    return;
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

QString CodeArtsMRProvider::scopeFromCategory(MergeRequestCategory category) const
{
    switch (category) {
    case MergeRequestCategory::ToMerge:
        return "assigned_to_me";
    case MergeRequestCategory::ToApprove:
        return "need_my_approve";
    case MergeRequestCategory::ToReview:
        return "need_my_review";
    case MergeRequestCategory::Created:
        return "created_by_me";
    default:
        return "assigned_to_me";
    }
}

QString CodeArtsMRProvider::stateFromQuery(MergeRequestState state) const
{
    switch (state) {
    case MergeRequestState::Opened:
        return "opened";
    case MergeRequestState::Closed:
        return "closed";
    case MergeRequestState::Merged:
        return "merged";
    case MergeRequestState::All:
        return "all";
    default:
        return "opened";
    }
}
