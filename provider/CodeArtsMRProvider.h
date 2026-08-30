#pragma once

#include "IMRProvider.h"

class CodeArtsMRProvider : public IMRProvider
{
    Q_OBJECT
public:
    explicit CodeArtsMRProvider(QObject* parent = nullptr);
    ProviderType providerType() const override;
    void refresh() override;
};
