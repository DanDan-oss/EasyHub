#pragma once

#include <QObject>
#include <QList>
#include <QQueue>
#include <QStringList>

#include "model/Notification.h"
#include "model/NotificationModel.h"

class NotificationService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(NotificationModel* model READ model CONSTANT)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)

public:
    static NotificationService& instance();
    NotificationModel* model();
    int unreadCount() const;
    void push(const Notification& notification);
    void push(const QList<Notification>& notifications);
    void finishPopupBatch();

    Q_INVOKABLE void activate(const QString& id);
    Q_INVOKABLE void consume(const QString& id);

signals:
    void unreadCountChanged();
    void popupBatchReady(const QStringList& notificationIds);
    void mergeRequestActivated(int providerType, const QString& repositoryId, int iid);

private:
    explicit NotificationService(QObject* parent = nullptr);
    void enqueuePopupBatch(const QStringList& notificationIds);
    void showNextPopupBatch();
    NotificationService(const NotificationService&) = delete;
    NotificationService& operator=(const NotificationService&) = delete;

private:
    NotificationModel m_model;
    QQueue<QStringList> m_popupQueue;
    bool m_popupShowing = false;
};
