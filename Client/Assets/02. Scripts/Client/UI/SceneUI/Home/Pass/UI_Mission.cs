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

        foreach (var passMissionData in Managers.Data.PassMissionDataDict)
        {
            switch(passMissionData.Value.Type)
            {
                case (int)Define.MissionType.DailyMission:
                    {
                        var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.MissionType.DailyMission - 1));
                        missionDatas.TryAdd(passMissionData.Value.Index, ui);
                        ui.Init();
                        ui.SetMissionIndex(passMissionData.Value.Index);
                        ui.SetTitle(passMissionData.Value.Mission_Name);
                        ui.SetDescription(passMissionData.Value.Mission_Description);
                        ui.SetExp(passMissionData.Value.Reward_Exp);
                        ui.SetRequired(passMissionData.Value.Required_Count);
                        ui.SetType(Define.MissionType.DailyMission);
                        ui.SetReward(passMissionData.Value.Reward_Item, passMissionData.Value.Reward_Item_Amount);
                        ui.SetRewarded(false);
                    }
                    break;
                case (int)Define.MissionType.PassMission:
                    {
                        var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.MissionType.PassMission - 1));
                        missionDatas.TryAdd(passMissionData.Value.Index, ui);
                        ui.Init();
                        ui.SetMissionIndex(passMissionData.Value.Index);
                        ui.SetTitle(passMissionData.Value.Mission_Name);
                        ui.SetDescription(passMissionData.Value.Mission_Description);
                        ui.SetRequired(passMissionData.Value.Required_Count);
                        ui.SetExp(passMissionData.Value.Reward_Exp);
                        ui.SetType(Define.MissionType.PassMission);
                        ui.SetRewarded(false);
                    }
                    break;
            }
        }

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

        Managers.Network.GetPacketManager().SendUserMissionStateRequestPacket();

        Debug.Log("Send Mission Request Packet.");

        isInitialized = true;
    }

    public void ChangeMissionData(int idx, int count, bool isRewarded)
    {
        missionDatas[idx].SetMissionCounted(count);
        missionDatas[idx].SetRewarded(isRewarded);
    }
}
