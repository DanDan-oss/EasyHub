#pragma once

#include <QString>

struct MergeRequest
{
    int iid =0;
    QString title;
    QString projectName;
    QString sourceBranch;
    QString targetBranch;
    QString mrState;
    QString pipelineStatus;
    int addedLines = 0;
    int deletedLines = 0;
    int reviewedCount = 0;
    int reviewerCount=0;
    int approvedCount=0;
    int approverCount =0;
    QString webUrl;
};

