#pragma once

#include <QString>
#include "ProviderType.h"

struct ProviderAccount
{
    ProviderType type = ProviderType::Unknown;
    QString username;
    QString remoteUserId;       // 平台返回的用户ID
};
