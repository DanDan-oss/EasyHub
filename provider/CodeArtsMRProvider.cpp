#include "CodeArtsMRProvider.h"

CodeArtsMRProvider::CodeArtsMRProvider(CredentialStore* credentialStore, QObject* parent)
    : IMRProvider(parent), m_credentialStore(credentialStore)
{

}

ProviderType CodeArtsMRProvider::providerType() const
{
    return ProviderType::CodeArts;
}

void CodeArtsMRProvider::refresh()
{

/*
    QList<MergeRequest> mergeRequests;

    MergeRequest mr1;
    mr1.key.iid = 873;
    mr1.key.repositoryId = "codearts-test-repository";
    mr1.key.providerType = providerType();
    mr1.title = "Fix log overwrite bug";
    mr1.projectName = "MSIPTool";
    mr1.sourceBranch = "single-trunk-wxd";
    mr1.targetBranch = "ReleaseBranch_main";
    mr1.mrState = "Opened";
    mr1.pipelineStatus = "Success";
    mr1.addedLines = 10;
    mr1.deletedLines = 15;
    mr1.reviewedCount = 1;
    mr1.reviewerCount = 2;
    mr1.approvedCount = 0;
    mr1.approverCount = 1;
    mr1.webUrl = "https://example.com/mr/873";

    mergeRequests.append(mr1);
    emit mergeRequestsLoaded(providerType(), mergeRequests);
*/
    if(!m_credentialStore)
    {
        emit refreshFailed(providerType(), " Credential store is unavailable.");
        return;
    }
}
