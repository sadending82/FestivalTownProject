using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Pass : UI_Scene
{
    enum GameObjects
    {
        BG,
        ExitButton,
        TitleText,
        BuyPassButton,
        PassPanel,
        MissionButton,
    }

    bool isInitialized = false;
    Dictionary<int, UI_PassLevelItem> passItemDatas = new();

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_HomeStart>();
            var popUpUi = Managers.UI.ShowPopUpUI<UI_HomeScene>();
            ui.Init();
            ui.SetCustomizing();
            popUpUi.Init();
        }, Define.UIEvent.Click, true, true);

        Get<GameObject>((int)GameObjects.MissionButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_Mission>();
        });

        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().Init();

        foreach (var passListData in Managers.Data.PassListDataDict)
        {
            for (int i = 1; Managers.Data.PassLevelDataDict.ContainsKey(passListData.Value.Index * 100 + i); ++i)
            {
                int basicStartIdx = passListData.Value.Index * 100 + i;
                int plusStartIdx = passListData.Value.Index * 100 + i + 100;
                var basicData = Managers.Data.PassLevelDataDict[basicStartIdx];
                var plusData = Managers.Data.PassLevelDataDict[plusStartIdx];

                var itemUi = AddItem(basicData.Reward_Item_Index, plusData.Reward_Item_Index, basicData.Index, plusData.Index, basicData.Level);

                passItemDatas.TryAdd(basicStartIdx, itemUi);
                passItemDatas.TryAdd(plusStartIdx, itemUi);
            }
        }

        Managers.Network.GetPacketManager().SendUserPassStateRequestPacket();

        Debug.Log("Send Pass Request Packet");

        isInitialized = true;
    }

    public void SetLevel(int level)
    {
        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().SetLevel(level);
    }

    public void SetProgress(int exp, int maxExp)
    {
        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().SetProgress(exp, maxExp);
    }

    public UI_PassLevelItem AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        return Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().AddItem(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }

    public void SetPassDataByIdx(int idx, bool isRewarded)
    {
        passItemDatas[idx].SetDataByIndex(idx, isRewarded);
    }
}
