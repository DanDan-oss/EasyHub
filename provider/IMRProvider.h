#pragma once

#include <QObject>
#include <QList>

#include "../model/MergeRequest.h"
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
    virtual void refresh() = 0;

signals:
    void mergeRequestsLoaded(ProviderType type, const QList<MergeRequest>& mergeRequests);
    void refreshFailed(ProviderType type, const QString& message);
};
