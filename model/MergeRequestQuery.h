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

inline QString mergeRequestStateToString(MergeRequestState state)
{
    switch (state)
    {
    case MergeRequestState::Opened:
        return "opened";
    case MergeRequestState::Closed:
        return "closed";
    case MergeRequestState::Merged:
        return "merged";
    case MergeRequestState::All:
        return "all";
    default:
        return {};
    }
}
