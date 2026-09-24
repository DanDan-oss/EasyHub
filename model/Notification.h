#pragma once
#include <QString>
#include "ProviderType.h"

enum class NotificationType
{
    MergeRequest
};

struct Notification
{
    QString id;
    NotificationType type = NotificationType::MergeRequest;
    QString title;
    QString message;
    ProviderType providerType = ProviderType::Unknown;
    QString repositoryId;
    int iid = 0;

};
