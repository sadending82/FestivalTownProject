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

        // 일일 미션에는 프로그래스가 있음.
        var progressUI = Managers.UI.MakeSubItem<UI_DailyProgress>(missionPanel.GetCategoryContent(0));
        progressUI.Init();

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
        missionDatas[idx].SetMissionCounted(count);
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
}
