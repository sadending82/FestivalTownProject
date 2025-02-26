using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Mission : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        MissionPanel,
        ExitButton,
    }

    bool isInitialized = false;
    Dictionary<int, UI_MissionData> missionDatas = new();
    int uncompletedPassMissionCount = 0;

    int dailyProgress = 0;
    int dailyProgressMax = 100;
    UI_DailyProgress _progressUI = null;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        var missionPanel = Get<GameObject>((int)GameObjects.MissionPanel).GetComponent<UI_MissionPanel>();
        missionPanel.Init();

        // ���� �̼ǿ��� ���α׷����� ����.
        var progressUI = Managers.UI.MakeSubItem<UI_DailyProgress>(missionPanel.GetCategoryContent(0));
        progressUI.Init();

        _progressUI = progressUI;

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

        Managers.Network.GetPacketManager().SendUserMissionStateRequestPacket();

        Debug.Log("Send Mission Request Packet.");

        isInitialized = true;
    }

    public UI_MissionData ChangeMissionData(int idx, int count, bool isRewarded)
    {
        Managers.Data.PassMissionDataDict.TryGetValue(idx, out var passMissionData);
        if (passMissionData == null) return null;

        missionDatas[idx].SetMissionCounted(count);

        if (passMissionData.Type == (int)Define.MissionType.DailyMission)
        {
            if (missionDatas[idx].IsRewarded() == false && isRewarded == true)
            {
                dailyProgress += passMissionData.Reward_Exp;
                if (dailyProgress > dailyProgressMax) dailyProgress = dailyProgressMax;
                _progressUI.SetProgress(dailyProgress);
            }
        }

        missionDatas[idx].SetRewarded(isRewarded);

        return missionDatas[idx];
    }

    public UI_MissionData AddMission(int missionIdx, int count, bool isRewarded)
    {
        Managers.Data.PassMissionDataDict.TryGetValue(missionIdx , out var passMissionData);

        var missionPanel = Get<GameObject>((int)GameObjects.MissionPanel).GetComponent<UI_MissionPanel>();

        switch (passMissionData.Type)
        {
            case (int)Define.MissionType.DailyMission:
                {
                    var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.MissionType.DailyMission - 1));
                    missionDatas.TryAdd(passMissionData.Index, ui);
                    ui.Init();
                    ui.SetMissionIndex(passMissionData.Index);
                    ui.SetTitle(passMissionData.Mission_Name);
                    ui.SetDescription(passMissionData.Mission_Description);
                    ui.SetExp(passMissionData.Reward_Exp);
                    ui.SetRequired(passMissionData.Required_Count);
                    ui.SetType(Define.MissionType.DailyMission);
                    ui.SetReward(passMissionData.Reward_Item, passMissionData.Reward_Item_Amount);
                    ui.SetMissionCounted(count);
                    ui.SetRewarded(isRewarded);
                    if(isRewarded)
                    {
                        dailyProgress += passMissionData.Reward_Exp;
                        if (dailyProgress > dailyProgressMax) dailyProgress = dailyProgressMax;
                        _progressUI.SetProgress(dailyProgress);
                    }
                    return ui;
                }
            case (int)Define.MissionType.PassMission:
                {
                    var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.MissionType.PassMission - 1));
                    missionDatas.TryAdd(passMissionData.Index, ui);
                    ui.Init();
                    ui.SetMissionIndex(passMissionData.Index);
                    ui.SetTitle(passMissionData.Mission_Name);
                    ui.SetDescription(passMissionData.Mission_Description);
                    ui.SetRequired(passMissionData.Required_Count);
                    ui.SetExp(passMissionData.Reward_Exp);
                    ui.SetType(Define.MissionType.PassMission);
                    ui.SetMissionCounted(count);
                    ui.SetRewarded(isRewarded);
                    if (!isRewarded)
                    {
                        uncompletedPassMissionCount++;
                    }
                    return ui;
                }
            default:
                return null;
        }
    }

    public void SetRecentPosition(UI_MissionData msdataUi)
    {
        if (msdataUi == null) return;

        uncompletedPassMissionCount--;
        missionDatas[msdataUi.GetMissionIndex()].transform.SetSiblingIndex(uncompletedPassMissionCount);
    }

    public void UpdateAllDatas()
    {
        foreach(var ui in missionDatas.Values)
        {
            Destroy(ui.gameObject);
        }
        missionDatas.Clear();

        Managers.Network.GetPacketManager().SendUserMissionStateRequestPacket();

        Debug.Log("Send Mission Request Packet.");
    }
}
