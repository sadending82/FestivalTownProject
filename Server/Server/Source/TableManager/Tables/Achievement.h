#pragma once
#pragma pack (push, 1)
struct AchievementInfo {
    int id;

    int progress;
    int target;

    int reward_item;

    bool isRewarded;
};

#pragma pack (pop)