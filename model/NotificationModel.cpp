#include "NotificationModel.h"


NotificationModel::NotificationModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int NotificationModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return m_notifications.size();
}

QVariant NotificationModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};
    if(index.row() < 0 || index.row() >= m_notifications.size())
        return {};
    const Notification& notification = m_notifications.at(index.row());
    switch (role) {
    case IdRole:
        return notification.id;
    case TypeRole:
        return static_cast<int>(notification.type);
    case TitleRole:
        return notification.title;
    case MessageRole:
        return notification.message;
    case ProviderTypeRole:
        return static_cast<int>(notification.providerType);
    case RepositoryIdRole:
        return notification.repositoryId;
    case IidRole:
        return notification.iid;
    default:
        return {};
    }
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
    return {
        {IdRole, "notificationId"},
        {TypeRole, "notificationType"},
        {TitleRole, "title"},
        {MessageRole, "message"},
        {ProviderTypeRole, "providerType"},
        {RepositoryIdRole, "repositoryId"},
        {IidRole, "iid"}
    };
}

const Notification* NotificationModel::notification(const QString& id) const
{
    for(const Notification& notification : m_notifications)
        if(notification.id == id)
            return &notification;
    return nullptr;
}

void NotificationModel::addNotification(const Notification& notification)
{
    const int row = m_notifications.size();
    beginInsertRows(QModelIndex(), row, row);
    m_notifications.append(notification);
    endInsertRows();
}

void NotificationModel::removeNotification(const QString& id)
{
    for(int i = 0; i < m_notifications.size(); ++i)
    {
        if(m_notifications.at(i).id != id)
            continue;
        beginRemoveRows(QModelIndex(), i, i);
        m_notifications.removeAt(i);
        endRemoveRows();
        return;
    }
}

bool NotificationModel::contains(const QString& id) const
{
    for(const Notification& notification : m_notifications)
        if(notification.id == id)
            return true;
    return false;
}

int NotificationModel::count() const
{
    return m_notifications.size();
}

QVariantMap NotificationModel::get(const QString& id) const
{
    const Notification* item = notification(id);
    if(!item)
        return {};
    return {
        {"id", item->id},
        {"type", static_cast<int>(item->type)},
        {"title", item->title},
        {"message", item->message},
        {"providerType", static_cast<int>(item->providerType)},
        {"repositoryId", item->repositoryId},
        {"iid", item->iid}
    };
}
