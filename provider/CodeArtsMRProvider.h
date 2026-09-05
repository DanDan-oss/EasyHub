#pragma once
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "IMRProvider.h"
#include "../service/CredentialStore.h"

class CodeArtsMRProvider : public IMRProvider
{
    Q_OBJECT
public:
    explicit CodeArtsMRProvider(CredentialStore* credentialStore, QObject* parent = nullptr);
    ProviderType providerType() const override;
    void refresh() override;
private:
    void handleRefreshReply(const QByteArray& body, int statusCode);
private:
    QNetworkAccessManager m_networkManager;
    CredentialStore* m_credentialStore = nullptr;
};
