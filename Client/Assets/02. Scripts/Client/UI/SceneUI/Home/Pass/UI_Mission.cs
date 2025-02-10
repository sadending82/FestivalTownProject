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

        // ���� �̼ǿ��� ���α׷����� ����.
        var progressUI = Managers.UI.MakeSubItem<UI_DailyProgress>(missionPanel.GetCategoryContent(0));
        progressUI.Init();

        foreach (var passMissionData in Managers.Data.PassMissionDataDict)
        {
            switch(passMissionData.Value.Type)
            {
                case (int)Define.PassType.DailyMission:
                    {
                        var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.PassType.DailyMission - 1));
                        missionDatas.TryAdd(passMissionData.Value.Index, ui);
                        ui.Init();
                        ui.SetTitle(passMissionData.Value.Mission_Name);
                        ui.SetDescription(passMissionData.Value.Mission_Description);
                        ui.SetExp(passMissionData.Value.Reward_Exp);
                        ui.SetRequired(passMissionData.Value.Required_Count);
                        ui.SetReward(passMissionData.Value.Reward_Item, passMissionData.Value.Reward_Item_Amount);
                    }
                    break;
                case (int)Define.PassType.PassMission:
                    {
                        var ui = Managers.UI.MakeSubItem<UI_MissionData>(missionPanel.GetCategoryContent((int)Define.PassType.PassMission - 1));
                        missionDatas.TryAdd(passMissionData.Value.Index, ui);
                        ui.Init();
                        ui.SetTitle(passMissionData.Value.Mission_Name);
                        ui.SetDescription(passMissionData.Value.Mission_Description);
                        ui.SetRequired(passMissionData.Value.Required_Count);
                        ui.SetExp(passMissionData.Value.Reward_Exp);
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
    }
}
