#pragma once

#include <QObject>
#include <QList>

#include "../model/MergeRequest.h"
#include "../model/MergeRequestQuery.h"
#include "../model/ProviderType.h"

class IMRProvider : public QObject
{
    Q_OBJECT
public:
    explicit IMRProvider(QObject* parent = nullptr)
        :QObject(parent)
    {
    }
    ~IMRProvider() override = default;
    virtual ProviderType providerType() const = 0;
    virtual void refresh(const MergeRequestQuery& query, quint64 requestId) = 0;
    virtual void loadMergeRequestDetail(const QString& repositoryId, int iid) = 0;
signals:
    void mergeRequestsLoaded(ProviderType type, quint64 requestId, const QList<MergeRequest>& mergeRequests);
    void refreshFailed(ProviderType type, quint64 requestId, const QString& message);
    void mergeRequestDetailLoaded(ProviderType type, const MergeRequestDetail& mergeRequest);
    void mergeRequestDetailFailed(ProviderType type, const QString& message);
};
