#pragma once
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "IMRProvider.h"
#include "../model/MergeRequestQuery.h"
#include "../service/CredentialStore.h"

class CodeArtsMRProvider : public IMRProvider
{
    Q_OBJECT
public:
    explicit CodeArtsMRProvider(CredentialStore* credentialStore, QObject* parent = nullptr);
    ProviderType providerType() const override;
    void refresh(const MergeRequestQuery& query, quint64 requestId) override;
    void loadMergeRequestDetail(const QString& repositoryId, int iid) override;
private:
    void handleRefreshReply(const QByteArray& body,  quint64 requestId, int statusCode);
    void handleDetailReply(const QByteArray& body);
    QString repositoryIdFromListItem(const QJsonObject& object);
    QString scopeFromCategory(MergeRequestCategory category) const;
    QString stateFromQuery(MergeRequestState state) const;
private:
    QNetworkAccessManager m_networkManager;
    CredentialStore* m_credentialStore = nullptr;
};
