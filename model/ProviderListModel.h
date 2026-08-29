#pragma once

#include <QAbstractListModel>
#include "ProviderType.h"
#include "../service/AccountManager.h"

class ProviderListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        ProviderTypeRole = Qt::UserRole +1,
        ProviderIdRole,
        ProviderNameRole,
        IconSourceRole,
        LoggedInRole
    };

    Q_PROPERTY(QString currentProviderId READ currentProviderId  NOTIFY currentProviderChanged)
    Q_PROPERTY(QString currentProviderName  READ currentProviderName NOTIFY currentProviderChanged)

    explicit ProviderListModel(AccountManager* accountManager, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString currentProviderId() const;
    QString currentProviderName() const;

    Q_INVOKABLE QString providerIdAt(int row) const;
    Q_INVOKABLE QString providerNameAt(int row) const;
    Q_INVOKABLE QString iconSourceAt(int row) const;
    Q_INVOKABLE bool loggedInAt(int row) const;
    Q_INVOKABLE bool setCurrentProvider(const QString& providerId);
signals:
    void currentProviderChanged();
private slots:
    void onProviderLoginStateChanged(ProviderType type);
private:
    int rowForProvider(ProviderType type) const;
private:
    AccountManager* m_accountManager = nullptr;
};
