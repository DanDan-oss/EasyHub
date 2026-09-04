#pragma once

#include <QNetworkAccessManager>
#include <QString>
#include <QVariantMap>

#include "IAuthProvider.h"

struct CodeArtsLoginCredential
{
    QString domainName;    // 华为云主体账号
    QString userName;      // 子账号
    QString password;
    QString region;         // 地区
};

class CodeArtsAuthProvider : public IAuthProvider
{
    Q_OBJECT
public:
    explicit CodeArtsAuthProvider(QObject* parent = nullptr);
    ProviderType providerType() const override;
    void login(const QVariantMap& parameters) override;
    void validateToken(const QString& accessToken) override;
private:
    QNetworkAccessManager m_networkManager;
};
