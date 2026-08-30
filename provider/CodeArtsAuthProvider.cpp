#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

#include "CodeArtsAuthProvider.h"
#include "IAuthProvider.h"

CodeArtsAuthProvider::CodeArtsAuthProvider(QObject* parent)
    : IAuthProvider(parent)
{
}

 ProviderType CodeArtsAuthProvider::providerType() const
{
     return ProviderType::CodeArts;
}


void CodeArtsAuthProvider::login(const QVariantMap& parameters)
{

    CodeArtsLoginCredential credential;
    credential.domainName = parameters.value("domainName").toString().trimmed();
    credential.userName = parameters.value("userName").toString().trimmed();
    credential.password = parameters.value("password").toString();
    credential.region = parameters.value("region").toString().trimmed();

    if(credential.domainName.isEmpty())
    {
        emit loginFailed(providerType(), " Domain cannot be empty.");
        return;
    }

    if(credential.userName.isEmpty())
    {
        emit loginFailed(providerType(), " UserName cannot be empty.");
        return;
    }
    if(credential.password.isEmpty())
    {
        emit loginFailed(providerType(), " Password cannot be empty.");
        return;
    }
    if(credential.region.isEmpty())
    {
        emit loginFailed(providerType(), " Region cannot be empty.");
        return;
    }

    // 设置消息头
    const QString url = QString("https://iam.%1.myhuaweicloud.com/v3/auth/tokens").arg(credential.region.trimmed());
    QNetworkRequest request{QUrl(url)};
    request.setRawHeader("Content-Type", "application/json;charset=utf8");

    // 拼接消息体
    QJsonObject domain;
    domain["name"] = credential.domainName.trimmed();

    QJsonObject user;
    user["domain"] = domain;
    user["name"] = credential.userName.trimmed();
    user["password"] = credential.password;

    QJsonObject password;
    password["user"] = user;

    QJsonObject identity;
    identity["methods"] = QJsonArray{"password"};
    identity["password"] = password;

    QJsonObject scope;
    scope["domain"] = domain;

    QJsonObject auth;
    auth["identity"] = identity;
    auth["scope"] = scope;

    QJsonObject root;
    root["auth"] = auth;

    const QByteArray body = QJsonDocument(root).toJson(QJsonDocument::Compact);

    qDebug() << "=================";
    // 使用 QNetworkAccessManager 请求IAM Token
    QNetworkReply* reply = m_networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, [this, reply, username = credential.userName.trimmed()]()
    {
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // 登录失败
        if(reply->error() != QNetworkReply::NoError)
        {
            const QString message = QString("CodeArts login failed. HTTP %1:%2").arg(statusCode).arg(reply->errorString());
            reply->deleteLater();
            emit loginFailed(providerType(), message);
            return;
        }

        // 登录成功,获取token
        const QByteArray token = reply->rawHeader("X-Subject-Token");
        reply->deleteLater();
        if(token.isEmpty())
        {
            emit loginFailed(providerType(), " Token was not returned By IAM.");
            return;
        }
        emit loginSucceeded(providerType(), username, QString::fromUtf8(token));
    });

}
