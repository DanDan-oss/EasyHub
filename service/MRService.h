#pragma once

#include <QObject>
#include <QHash>
#include <QSet>
#include <QTimer>
#include "../model/MRListModel.h"
#include "../model/ProviderType.h"
#include "../service/AccountManager.h"
#include "../provider/IMRProvider.h"
#include "../model/MergeRequestQuery.h"
#include "../config/AppConfig.h"

class MRService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MRListModel* model READ model CONSTANT)
    Q_PROPERTY(int toMergeCount READ toMergeCount  NOTIFY countsChanged)
    Q_PROPERTY(int toApproveCount READ toApproveCount NOTIFY countsChanged)
    Q_PROPERTY(int toReviewCount READ toReviewCount NOTIFY countsChanged)
    Q_PROPERTY(int createdCount READ createdCount NOTIFY countsChanged)

public:
    explicit MRService(AccountManager* accountManager, QObject* parent = nullptr);
    MRListModel* model();
    void registerProvider(IMRProvider* provider);
    Q_INVOKABLE void loadMergeRequestDetail(const QString& repositoryId, int iid);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setCategory(int category);
    Q_INVOKABLE void setState(int state);
    Q_INVOKABLE void openMergeRequest(int providerType, const QString& repositoryId, int iid);
    int toMergeCount() const;
    int toApproveCount() const;
    int toReviewCount() const;
    int createdCount() const;
private:
    IMRProvider* currentProvider() const;
    void updateModel();
    bool belongsToCategory(const MergeRequest& mergeRequest, MergeRequestCategory category) const;
    int countByCategory(MergeRequestCategory category) const;

signals:
    void mergeRequestDetailLoaded(int iid, const QString& title, const QString& projectName, const QString& sourceBranch, \
                                  const QString& targetBranch, const QString& state, const QString& pipelineStatus, const QString& webUrl);
    void mergeRequestsAdded(ProviderType type, const QList<MergeRequest>& mergeRequests);
    void countsChanged();
private slots:
    void onCurrentProviderChanged(ProviderType type);
    void onMergeRequestsLoaded(ProviderType type, quint64 requestId, const QList<MergeRequest>& mergeRequests);
    void onRefreshFailed(ProviderType type, quint64 requestId, const QString& message);
    void onMergeRequestDetailLoaded(ProviderType type, const MergeRequestDetail& detail);
    void onMergeRequestDetailFailed(ProviderType type, const QString& message);
    void onQueriedMergeRequestsLoaded(ProviderType type, quint64 requestId, MergeRequestState state, const QList<MergeRequest>& mergeRequests);
    void onQueryMergeRequestsFailed(ProviderType type, quint64 requestId, MergeRequestState state, const QString& error);
    void onProviderLoggedIn(ProviderType type);
private:
    MRListModel m_model;
    AccountManager* m_accountManager = nullptr;
    QHash<ProviderType, IMRProvider*> m_providers;
    QHash<ProviderType, QList<MergeRequest>> m_mergeRequests;       // 保存各个平台最近一次同步完成后的MR集合
    MergeRequestQuery m_query;
    QList<MergeRequest> m_queryMergeRequests;
    quint64 m_requestId = 0;
    quint64 m_queryRequestId = 0;
    QHash<ProviderType, quint64> m_refreshRequestIds;
    QHash<ProviderType, QSet<QString>> m_mergeRequestSnapshots;
    QSet<ProviderType> m_refreshingProviders;
    QTimer m_refreshTimer;
};
