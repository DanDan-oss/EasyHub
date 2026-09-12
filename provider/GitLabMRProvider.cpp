#include "GitLabMRProvider.h"

GitLabMRProvider::GitLabMRProvider(QObject* parent)
    : IMRProvider(parent)
{
}

ProviderType GitLabMRProvider::providerType() const
{
    return ProviderType::GitLab;
}

void GitLabMRProvider::refresh(quint64 requestId)
{
    QList<MergeRequest> mergeRequests;

    MergeRequest mr1;
    mr1.key.iid = 1001;
    mr1.key.providerType = providerType();
    mr1.key.repositoryId = "gitlab-test-project";
    mr1.title = "GitLab test merge request";
    mr1.projectName = "EasyHub-GitLab";
    mr1.sourceBranch = "aaaa";
    mr1.targetBranch = "main";
    mr1.mrState = "Opened";
    mr1.pipelineStatus = "Running";
    mr1.addedLines = 20;
    mr1.deletedLines = 4;
    mr1.reviewedCount = 0;
    mr1.reviewerCount = 1;
    mr1.approvedCount = 0;
    mr1.approverCount = 1;
    mr1.webUrl = "https://example.com/mr/873";

    mergeRequests.append(mr1);
    emit mergeRequestsLoaded(providerType(), requestId, mergeRequests);
}

void GitLabMRProvider::loadMergeRequestDetail(const QString& repositoryId, int iid)
{
    Q_UNUSED(repositoryId);
    Q_UNUSED(iid);

    emit mergeRequestDetailFailed(providerType(), "GitLab MR detail is not implemented.");

}
