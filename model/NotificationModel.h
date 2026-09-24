#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QVariantMap>
#include "Notification.h"

class NotificationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum NotificationRole
    {
        IdRole = Qt::UserRole +1,
        TypeRole,
        TitleRole,
        MessageRole,
        ProviderTypeRole,
        RepositoryIdRole,
        IidRole
    };
    explicit NotificationModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addNotification(const Notification& notification);
    void removeNotification(const QString& id);

    const Notification* notification(const QString& id) const;
    bool contains(const QString& id) const;
    int count() const;

    Q_INVOKABLE QVariantMap get(const QString& id) const;
private:
    QList<Notification> m_notifications;
};
