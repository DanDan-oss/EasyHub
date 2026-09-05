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

void CodeArtsMRProvider::refresh()
{
    if(!m_credentialStore)
    {
        emit refreshFailed(providerType(), "Credential store is unavailable.");
        return;
    }
    if(!m_credentialStore->isAccessTokenValid(providerType()))
    {
        emit refreshFailed(providerType(), "Access token is unavailable or expired.");
        return;
    }
    const AccessCredential* credential = m_credentialStore->credential(providerType());
    if(!credential)
    {
        emit refreshFailed(providerType(), "Access credential is unavailable.");
        return;
    }
    if(credential->accessToken.isEmpty())
    {
        emit refreshFailed(providerType(), "Access token is empty.");
        return;
    }
    if(credential->region.isEmpty())
    {
        emit refreshFailed(providerType(), "region is unavailable.");
        return;
    }

    // 设置消息头
    QUrl url = QString("https://codehub-ext.%1.myhuaweicloud.com/v4/merge-requests").arg(credential->region);
    QUrlQuery query;
    query.addQueryItem("state", "opened");
    query.addQueryItem("scope", "all");
    query.addQueryItem("order_by", "updated_at");
    query.addQueryItem("sort", "desc");
    query.addQueryItem("offset", "0");
    query.addQueryItem("limit", "50");
    url.setQuery(query);
    qDebug() << "Refreshing CodeArts merge requests."  << "Region:" << credential->region;
    qDebug() << "CodeArts MR request. " << url;

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json;charset=utf8");
    request.setRawHeader("X-Auth-Token", credential->accessToken.toUtf8());

    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
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
            {
                qWarning() << "CodeArts error response:" << QString::fromUtf8(body);
            }
            emit refreshFailed(providerType(), reply->errorString());
            return;
        }
        handleRefreshReply(body, statusCode);
    });

    return;
}

void CodeArtsMRProvider::handleRefreshReply(const QByteArray& body, int statusCode)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        emit refreshFailed(providerType(), "Invalid MR response.");
        return;
    }
    if(!document.isArray())
    {
        emit refreshFailed(providerType(), "Unexpected MR response.");
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
        mergeRequest.key.iid = object.value("iid").toInt();
        mergeRequest.title = object.value("title").toString();
        mergeRequest.sourceBranch = object.value("source_branch").toString();
        mergeRequest.targetBranch = object.value("target_branch").toString();
        mergeRequest.mrState = object.value("state").toString();
        mergeRequest.pipelineStatus = object.value("pipeline_status").toString();
        mergeRequest.addedLines = object.value("added_lines").toInt();
        mergeRequest.deletedLines = object.value("removed_lines").toInt();
        mergeRequest.webUrl = object.value("web_url").toString();
        mergeRequest.projectName = object.value("product_name").toString();
        mergeRequests.append(std::move(mergeRequest));
    }
    qDebug() << providerTypeToString( providerType()) << " MR Loaded:" << mergeRequests.size();
    emit mergeRequestsLoaded( providerType(), mergeRequests);
    return;
}
