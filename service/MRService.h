#pragma once

#include <QObject>
#include <QHash>
#include "../model/MRListModel.h"
#include "../model/ProviderType.h"
#include "../service/AccountManager.h"
#include "../provider/IMRProvider.h"

class MRService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MRListModel* model READ model CONSTANT)

public:
    explicit MRService(AccountManager* accountManager, QObject* parent = nullptr);
    MRListModel* model();
    void registerProvider(IMRProvider* provider);
    Q_INVOKABLE void refresh();
private slots:
    void onCurrentProviderChanged(ProviderType type);
    void onMergeRequestsLoaded(ProviderType type, const QList<MergeRequest>& mergeRequests);
    void onRefreshFailed(ProviderType type, const QString& message);
private:
    IMRProvider* currentProvider() const;
private:
    MRListModel m_model;
    AccountManager* m_accountManager = nullptr;
    QHash<ProviderType, IMRProvider*> m_providers;
};
