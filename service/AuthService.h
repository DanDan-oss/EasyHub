#pragma once

#include <QObject>
#include <QString>

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(QObject* parent=nullptr);
    Q_INVOKABLE void login(const QString& username, const QString& password);

signals:
    void loginSucceeded();
    void loginFailed(const QString& message);
};
