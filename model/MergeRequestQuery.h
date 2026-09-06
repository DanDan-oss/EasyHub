#pragma once
#include <QString>

enum class MergeRequestCategory
{
    ToMerge,
    ToApprove,
    ToReview,
    Created
};

enum class MergeRequestState
{
    Opened,
    Closed,
    Merged,
    All
};

struct MergeRequestQuery
{
    MergeRequestCategory category = MergeRequestCategory::ToMerge;
    MergeRequestState state = MergeRequestState::Opened;
};
