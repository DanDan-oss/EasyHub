#include "MRService.h"

MRService::MRService(QObject* parent)
    : QObject(parent), m_model(this)
{

}

MRListModel* MRService::model()
{
    return &this->m_model;
}

void MRService::loadTestData()
{
    this->m_model.clear();

    MergeRequest mr1;
    mr1.iid = 873;
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
    mr1.webUrl =
        "https://example.com/mr/873";

    m_model.addMergeRequest(mr1);

    MergeRequest mr2;
    mr2.iid = 912;
    mr2.title = "Fix login exception";
    mr2.projectName = "WeSpace";
    mr2.sourceBranch = "feature/login-fix";
    mr2.targetBranch = "master";
    mr2.mrState = "Opened";
    mr2.pipelineStatus = "Running";
    mr2.addedLines = 8;
    mr2.deletedLines = 3;
    mr2.reviewedCount = 0;
    mr2.reviewerCount = 2;
    mr2.approvedCount = 0;
    mr2.approverCount = 1;

    m_model.addMergeRequest(mr2);

    MergeRequest mr3;
    mr3.iid = 1024;
    mr3.title = "Update configuration loader";
    mr3.projectName = "eAPP610";
    mr3.sourceBranch = "feature/config";
    mr3.targetBranch = "ICP-D";
    mr3.mrState = "Opened";
    mr3.pipelineStatus = "Failed";
    mr3.addedLines = 23;
    mr3.deletedLines = 11;
    mr3.reviewedCount = 2;
    mr3.reviewerCount = 2;
    mr3.approvedCount = 1;
    mr3.approverCount = 1;

    m_model.addMergeRequest(mr3);
}
