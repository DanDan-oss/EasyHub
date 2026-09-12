#pragma once
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QJsonArray>
#include "IMRProvider.h"
#include "../service/AccountManager.h"
#include "../service/CredentialStore.h"

// 保存MR刷新过程中的临时状态.
// 先获取个人MR列表,再遍历MR列表中的每个MR详情.所有的请求完成后生成最终的MR列表
struct RefreshContext
{
    quint64 requestId = 0;
    QList<MergeRequest> mergeRequests;      // 待检视、待合并、待审核去重后的MR集合
    QHash<QString, int> mergeRequestIndexes;    // 用于待检视、待合并、待审核中重复出现的同一个MR列表
    int pendingLists = 0;       // 尚未完成的List请求数量
    int pendingDetails = 0;     // 尚未完成的Detail的请求数量
};


class CodeArtsMRProvider : public IMRProvider
{
    Q_OBJECT
public:
    explicit CodeArtsMRProvider(CredentialStore* credentialStore, AccountManager* accountManager, QObject* parent = nullptr);
    ProviderType providerType() const override;
    void refresh(quint64 requestId) override;
    void loadMergeRequestDetail(const QString& repositoryId, int iid) override;
private:
    void handleDetailReply(const QByteArray& body);
    QString repositoryIdFromListItem(const QJsonObject& object);
    void requestMergeRequestList(quint64 requestId, const QString& scope);
    void handleMergeRequestListReply(const QByteArray& body, quint64 requestId, const QString& scope);
    void startDetailRequests(quint64 requestId);
    void requestMergeRequestDetail(quint64 requestId, int index);
    void completeRefreshIfReady(quint64 requestId);
    QString projectNameFromListItem(const QJsonObject& object);
    bool containsUser(const QJsonArray& users, const QString& remoteUserId) const;
    QString userState(const QJsonArray& users, const QString& remoteUserId) const;  //  读取用户再MR中的状态
    void parseMergeRequestRelations(MergeRequest& mergeRequest, const QJsonObject& object, const QString& remoteUserId) const;
private:
    QNetworkAccessManager m_networkManager;
    CredentialStore* m_credentialStore = nullptr;
    AccountManager* m_accountManager = nullptr;
    QHash<quint64, RefreshContext> m_refreshContexts;
};
