#include "AuthService.h"

#include <QDebug>

AuthService::AuthService(QObject* parent)
    : QObject(parent)
{

}

void AuthService::login(const QString& username, const QString& password)
{
    qDebug() << "AuthService::login()";
    if(username.isEmpty())
    {
        emit loginFailed("username cannot be empty.");
        return;
    }
    if(password.isEmpty())
    {
        emit loginFailed("password cannot be empty.");
        return;
    }
    qDebug() << "username:" << username;

    // 目前还没有真正连接CodeArts
    // 暂时认为输入用户名和密码登录成功
    emit loginSucceeded();
}

