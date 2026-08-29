#pragma once

#include <QString>
#include <array>
#include <algorithm>

enum class ProviderType
{
    CodeArts,
    GitLab,
    Unknown
};

struct ProviderDescriptor
{
    ProviderType type;
    const char* id;
    const char* displayName;
    const char* iconSource;
};

inline constexpr std::array<ProviderDescriptor, 2> kProviderDescriptors {{
    {
        ProviderType::CodeArts,
        "codearts",
        "CodeArts",
        "qrc:/qt/qml/EasyHub/resources/icons/codearts.svg"
    },
    {
        ProviderType::GitLab,
        "gitlab",
        "GitLab",
        "qrc:/qt/qml/EasyHub/resources/icons/gitlab.svg"
    }
}};

inline const ProviderDescriptor* providerDescriptor(ProviderType type)
{
    auto it = std::find_if(kProviderDescriptors.begin(), kProviderDescriptors.end(), [type](const ProviderDescriptor& provider){
        return provider.type == type;
    });

    if(it == kProviderDescriptors.end())
        return nullptr;
    return &(*it);
}

inline const ProviderDescriptor* providerDescriptor(const QString& id)
{
    auto it = std::find_if(kProviderDescriptors.begin(), kProviderDescriptors.end(), [&id](const ProviderDescriptor& provider){
        return id.compare(provider.id, Qt::CaseInsensitive) ==0;
    });

    if(it == kProviderDescriptors.end())
        return nullptr;
    return &(*it);
}



inline QString  providerTypeToString(ProviderType type)
{
    const ProviderDescriptor* provider = providerDescriptor(type);
    if(!provider)
        return "unknown";
    return provider->id;
}


inline ProviderType providerTypeFromString(const QString& value)
{
    const ProviderDescriptor* provider = providerDescriptor(value);
    if(!provider)
        return ProviderType::Unknown;
    return provider->type;
}

inline QString providerDisplayName(ProviderType type)
{
    const ProviderDescriptor* provider = providerDescriptor(type);
    if(!provider)
        return "unknown";
    return provider->displayName;
}

inline QString providerIconSource(ProviderType type)
{
    const ProviderDescriptor* provider = providerDescriptor(type);
    if(!provider)
        return {};
    return provider->iconSource;
}
