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

    QJsonObject project;
    project["name"] = credential.region;

    QJsonObject scope;
    scope["project"] = project;

    QJsonObject auth;
    auth["identity"] = identity;
    auth["scope"] = scope;

    QJsonObject root;
    root["auth"] = auth;

    const QByteArray body = QJsonDocument(root).toJson(QJsonDocument::Compact);

    // 使用 QNetworkAccessManager 请求IAM Token
    const QString username = credential.userName;
    const QString region = credential.region;
    QNetworkReply* reply = m_networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, [this, reply, username, region]()
    {
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // 登录失败
        if(reply->error() != QNetworkReply::NoError)
        {
            const QByteArray responseBody = reply->readAll();
            qWarning() << "CodeArts login failed:" << statusCode << reply->errorString();
            if(!responseBody.isEmpty())
                qWarning() << "CodeArts IAM error response:" << QString::fromUtf8(responseBody);
            const QString message = QString("CodeArts login failed. HTTP %1:%2").arg(statusCode).arg(reply->errorString());
            reply->deleteLater();
            emit loginFailed(providerType(), message);
            return;
        }

        // 登录成功,获取token
        const QByteArray token = reply->rawHeader("X-Subject-Token");
        const QByteArray respon = reply->readAll();
        reply->deleteLater();
        if(token.isEmpty())
        {
            emit loginFailed(providerType(), " Token was not returned By IAM.");
            return;
        }
        const QJsonDocument document = QJsonDocument::fromJson(respon);
        const QJsonObject tokenObject = document.object().value("token").toObject();
        const QString expires =  tokenObject.value("expires_at").toString();
        const QDateTime expiresAt = QDateTime::fromString(expires, Qt::ISODateWithMs);
        if(!expiresAt.isValid())
        {
            emit loginFailed(providerType(), "Invalid token expiration time returned by IAM");
            return;
        }
        AccessCredential accessCredential;
        accessCredential.accessToken = QString::fromUtf8(token);
        accessCredential.expiresAt = expiresAt;
        accessCredential.region = region;
        emit loginSucceeded(providerType(), username, accessCredential);
    });

}

void CodeArtsAuthProvider::validateToken(const QString& accessToken)
{
    if(accessToken.isEmpty())
    {
        emit tokenValidationFailed(providerType(), "Access token is empty.");
        return;
    }
    QNetworkRequest request{QUrl("https://iam.myhuaweicloud.com/v3/auth/tokens")};
    request.setRawHeader("X-Auth-Token", accessToken.toUtf8());
    request.setRawHeader("X-Subject-Token", accessToken.toUtf8());
    request.setRawHeader("Content-Type", "application/json;charset=utf8");
    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if(reply->error() != QNetworkReply::NoError)
        {
            const QString message = QString("Token validation failed. HTTP %1 : %2").arg(statusCode).arg(reply->errorString());
            reply->deleteLater();
            emit tokenValidationFailed(providerType(), message);
            return;
        }
        // 获取token有效性成功
        const QByteArray body = reply->readAll();
        reply->deleteLater();
        if(body.isEmpty())
        {
            emit tokenValidationFailed(providerType(), "Failed to token request  body.");
            return;
        }
        const QJsonDocument document = QJsonDocument::fromJson(body);
        const QJsonObject tokenObject = document.object().value("token").toObject();
        const QString username = tokenObject.value("user").toObject().value("name").toString();
        if(username.isEmpty())
        {
            emit tokenValidationFailed(providerType(), "Failed to obtain user information.");
            return;
        }
        emit tokenValidated(providerType(), username);
    });
}
