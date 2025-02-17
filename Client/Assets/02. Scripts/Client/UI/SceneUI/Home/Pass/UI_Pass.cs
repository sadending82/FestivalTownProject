using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Pass : UI_Scene
{
    enum GameObjects
    {
        BG,
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
        ExitButton,
        TitleText,
        PeriodText,
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
            ReturnToHome();
        }, Define.UIEvent.Click, true, true);

        Get<GameObject>((int)GameObjects.MissionButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_Mission>();
        });

        Get<GameObject>((int)GameObjects.BuyPassButton).BindEvent((PointerEventData) =>
        {
            // 패스 구매 버튼
        });

        var panelUI = Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>();
        panelUI.Init();
        

        foreach (var passListData in Managers.Data.PassListDataDict)
        {
            SetPeriod(passListData.Value.Open_Date, passListData.Value.Close_Date);
            panelUI.SetPassIdx(passListData.Key);

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

        SetGold(Managers.Data.GetGold());
        SetDiamond(Managers.Data.GetDiamond());

        Managers.Network.GetPacketManager().SendUserPassStateRequestPacket();

        isInitialized = true;
    }

    public void SetPeriod(int StartDay, int EndDay)
    {
        DateTime date = new DateTime(1900, 1, 1);
        DateTime startDate = date.AddDays(StartDay - 2);
        DateTime endDate = date.AddDays(EndDay - 2);

        Get<GameObject>((int)GameObjects.PeriodText).GetComponent<TMP_Text>().text =
            $"진행 기간 : {startDate.ToString("yyyy-MM-dd")} ~ {endDate.ToString("yyyy-MM-dd")}";
    }

    public void SetLevel(int level)
    {
        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().SetLevel(level);
    }

    public void SetProgress(int exp, int maxExp)
    {
        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().SetProgress(exp, maxExp);
    }

    public void SetDiamond(int value)
    {
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public void SetGold(int value)
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public UI_PassLevelItem AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        return Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().AddItem(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }

    public void SetPassItemRewarded(int idx, bool isRewarded)
    {
        Debug.Log($"SetReward {idx} to {isRewarded}");

        if (passItemDatas.ContainsKey(idx))
        {
            Debug.Log($"{idx} Contains.");
            passItemDatas[idx].SetItemRewarded(idx, isRewarded);
        }
    }

    public void ReturnToHome()
    {
        if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>() != null && Managers.UI.GetTopOfPopUPUI() == null)
        {
            Managers.UI.CloseSceneUI();
            var hui = Managers.UI.ShowSceneUI<UI_HomeStart>();
            hui.Init();
            hui.SetCustomizing();

            var popup = Managers.UI.ShowPopUpUI<UI_HomeScene>();
        }
    }

    public void Update()
    {

        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = Managers.Data.GetGold().ToString();
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = Managers.Data.GetDiamond().ToString();

        if(Input.GetKeyDown(KeyCode.Escape))
        {
            ReturnToHome();
        }
    }
}
