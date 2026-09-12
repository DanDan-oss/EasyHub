#pragma once

#include "IMRProvider.h"

class GitLabMRProvider : public IMRProvider
{
    Q_OBJECT

public:
    explicit GitLabMRProvider(QObject* parent = nullptr);
    ProviderType providerType() const override;
    void refresh(quint64 requestId) override;
    void loadMergeRequestDetail(const QString& repositoryId, int iid) override;
};
