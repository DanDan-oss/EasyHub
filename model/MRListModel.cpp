#include "MRListModel.h"


MRListModel::MRListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}


int MRListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return this->m_mergeRequests.size();
}

QVariant MRListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};
    if(index.row() <0 || index.row() >= this->m_mergeRequests.size() )
        return {};
    const MergeRequest& mr = this->m_mergeRequests.at(index.row());

    switch (role) {
    case MRListModel::Roles::IidRole:
        return mr.iid;
    case MRListModel::Roles::TitleRole:
        return mr.title;
    case MRListModel::Roles::ProjectNameRole:
        return mr.projectName;
    case MRListModel::Roles::SourceBranchRole:
        return mr.sourceBranch;
    case MRListModel::Roles::TargetBranchRole:
        return mr.targetBranch;
    case MRListModel::Roles::MRStateRole:
        return mr.mrState;
    case MRListModel::Roles::PipelineStatusRole:
        return mr.pipelineStatus;
    case MRListModel::Roles::AddedLinesRole:
        return mr.addedLines;
    case MRListModel::Roles::DeletedLinesRole:
        return mr.deletedLines;
    case MRListModel::Roles::ReviewedCountRole:
        return mr.reviewedCount;
    case MRListModel::Roles::ReviewerCountRole:
        return mr.reviewerCount;
    case MRListModel::Roles::ApprovedCountRole:
        return mr.approvedCount;
    case MRListModel::Roles::ApproverCountRole:
        return mr.approverCount;
    case MRListModel::Roles::WebUrlRole:
        return mr.webUrl;
    default:
        return {};
    }
}

QHash<int, QByteArray> MRListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MRListModel::Roles::IidRole] = "iid";
    roles[MRListModel::Roles::TitleRole] = "title";
    roles[MRListModel::Roles::ProjectNameRole] = "projectName";
    roles[MRListModel::Roles::SourceBranchRole] = "sourceBranch";
    roles[MRListModel::Roles::TargetBranchRole] = "targetBranch";
    roles[MRListModel::Roles::MRStateRole] = "mrState";
    roles[MRListModel::Roles::PipelineStatusRole] = "pipelineStatus";
    roles[MRListModel::Roles::AddedLinesRole] = "addedLines";
    roles[MRListModel::Roles::DeletedLinesRole] = "deletedLines";
    roles[MRListModel::Roles::ReviewedCountRole] = "reviewedCount";
    roles[MRListModel::Roles::ReviewerCountRole] = "reviewerCount";
    roles[MRListModel::Roles::ApprovedCountRole] = "approvedCount";
    roles[MRListModel::Roles::ApproverCountRole] = "approverCount";
    roles[MRListModel::Roles::WebUrlRole] = "webUrl";
    return roles;
}

void MRListModel::addMergeRequest(const MergeRequest& mr)
{
    const int row = this->m_mergeRequests.size();
    beginInsertRows(QModelIndex(), row, row);
    this->m_mergeRequests.append(mr);
    endInsertRows();
}

void MRListModel::clear()
{
    if(this->m_mergeRequests.isEmpty())
        return;
    beginResetModel();
    this->m_mergeRequests.clear();
    endResetModel();
}

const MergeRequest* MRListModel::mergeRequestAt(int row) const
{
    if(row >= this->m_mergeRequests.size() || row <0)
        return nullptr;
    return &this->m_mergeRequests.at(row);
}
