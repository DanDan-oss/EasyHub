#pragma once

#include <QString>
#include "ProviderType.h"

struct ProviderAccount
{
    ProviderType type = ProviderType::Unknown;
    QString username;
};
