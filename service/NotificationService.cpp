#include <QDebug>
#include "NotificationService.h"

NotificationService::NotificationService(QObject *parent)
    : QObject(parent)
{
}

NotificationService& NotificationService::instance()
{
    static NotificationService instance;
    return instance;
}

NotificationModel* NotificationService::model()
{
    return &m_model;
}

int NotificationService::unreadCount() const
{
    return m_model.count();
}

void NotificationService::push(const Notification& notification)
{
    if(m_model.contains(notification.id))
        return;
    m_model.addNotification(notification);
    qDebug() << "Notification added:" << notification.id;
    qDebug() << "Unread Notifications:" << m_model.count();
    emit unreadCountChanged();
    enqueuePopupBatch({ notification.id });
}

void NotificationService::push(const QList<Notification>& notifications)
{
    QStringList notificationIds;
    for(const Notification& notification : notifications)
    {
        if(m_model.contains(notification.id))
            continue;
        m_model.addNotification(notification);
        notificationIds.append(notification.id);
        qDebug() << "Notification added:" << notification.id;
    }
    if(notificationIds.isEmpty())
        return;
    qDebug() << "Unread Notifications:" << m_model.count();
    emit unreadCountChanged();
    enqueuePopupBatch(notificationIds);
}

void NotificationService::finishPopupBatch()
{
    if(!m_popupShowing)
        return;
    m_popupShowing = false;
    qDebug() << "Popup batch finished";
    showNextPopupBatch();

}

void NotificationService::activate(const QString& id)
{
    const Notification* notification = m_model.notification(id);
    if(!notification)
        return;

    const Notification item = *notification;

    switch (item.type) {
    case NotificationType::MergeRequest:
        emit mergeRequestActivated(static_cast<int>(item.providerType), item.repositoryId, item.iid);
        break;
    default:
        break;
    }
    consume(id);
}

void NotificationService::consume(const QString& id)
{
    if(!m_model.contains(id))
        return;
    m_model.removeNotification(id);
    qDebug() << "Notification consumed:" << id;
    qDebug() << "Unread Notifications:" << m_model.count();
    emit unreadCountChanged();
}

void NotificationService::enqueuePopupBatch(const QStringList& notificationIds)
{
    if(notificationIds.isEmpty())
        return;
    m_popupQueue.enqueue(notificationIds);
    qDebug() << "Notification batch queued:" << notificationIds.size();
    if(m_popupShowing)
        return;
    showNextPopupBatch();
}

void NotificationService::showNextPopupBatch()
{
    while(!m_popupQueue.isEmpty())
    {
        const QStringList notificationIds = m_popupQueue.dequeue();
        QStringList unreadIds;
        for(const QString& id : notificationIds)
        {
            if(!m_model.contains(id))
                continue;
            unreadIds.append(id);
        }

        if(unreadIds.isEmpty())
            continue;
        m_popupShowing = true;
        qDebug() << "Popup batch ready:" << unreadIds.size();
        emit popupBatchReady(unreadIds);
        return;
    }

    m_popupShowing = false;
    qDebug() << "Popup queue idle";
}
