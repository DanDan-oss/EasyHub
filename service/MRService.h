#pragma once

#include <QObject>
#include <QHash>
#include "../model/MRListModel.h"
#include "../model/ProviderType.h"
#include "../service/AccountManager.h"
#include "../provider/IMRProvider.h"
#include "../model/MergeRequestQuery.h"

class MRService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MRListModel* model READ model CONSTANT)

public:
    explicit MRService(AccountManager* accountManager, QObject* parent = nullptr);
    MRListModel* model();
    void registerProvider(IMRProvider* provider);
    Q_INVOKABLE void loadMergeRequestDetail(const QString& repositoryId, int iid);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setCategory(int category);
    Q_INVOKABLE void setState(int stats);
signals:
    void mergeRequestDetailLoaded(int iid, const QString& title, const QString& projectName, const QString& sourceBranch, \
                                  const QString& targetBranch, const QString& state, const QString& pipelineStatus, const QString& webUrl);
private slots:
    void onCurrentProviderChanged(ProviderType type);
    void onMergeRequestsLoaded(ProviderType type, quint64 requestId, const QList<MergeRequest>& mergeRequests);
    void onRefreshFailed(ProviderType type, quint64 requestId, const QString& message);
    void onMergeRequestDetailLoaded(ProviderType type, const MergeRequestDetail& detail);
    void onMergeRequestDetailFailed(ProviderType type, const QString& message);
private:
    IMRProvider* currentProvider() const;
    void updateModel();
    bool matchesCategory(const MergeRequest& mergeRequest) const;
private:
    MRListModel m_model;
    AccountManager* m_accountManager = nullptr;
    QHash<ProviderType, IMRProvider*> m_providers;
    QHash<ProviderType, QList<MergeRequest>> m_mergeRequests;       // 保存各个平台最近一次同步完成后的MR集合
    MergeRequestQuery m_query;
    quint64 m_refreshRequestId = 0;
};
