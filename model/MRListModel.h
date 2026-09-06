#pragma once

#include <QAbstractListModel>
#include <QList>

#include "MergeRequest.h"

class MRListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles{
        IidRole =Qt::UserRole+1,
        TitleRole,
        RepositoryIdRole,
        ProjectNameRole,
        SourceBranchRole,
        TargetBranchRole,
        MRStateRole,
        PipelineStatusRole,
        AddedLinesRole,
        DeletedLinesRole,
        ReviewedCountRole,
        ReviewerCountRole,
        ApprovedCountRole,
        ApproverCountRole,
        WebUrlRole
    };

    explicit MRListModel(QObject* parent=nullptr);

public:
    // 适配QML
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public:
    void addMergeRequest(const MergeRequest& mr);
    void clear();
    const MergeRequest* mergeRequestAt(int row) const;

private:
    QList<MergeRequest> m_mergeRequests;
};
