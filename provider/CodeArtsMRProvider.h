#pragma once
#include <QNetworkAccessManager>
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
    QNetworkAccessManager m_networkManager;
    CredentialStore* m_credentialStore = nullptr;
};
