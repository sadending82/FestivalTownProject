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
    }

    bool isInitialized = false;

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

        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().Init();


        for (int i = 0; Managers.Data.PassListDataDict.ContainsKey((int)Define.StartIdx.PassPlusListStartIdx + i); ++i)
        {
            var basicData = Managers.Data.PassListDataDict[(int)Define.StartIdx.PassBasicListStartIdx + i];
            var plusData = Managers.Data.PassListDataDict[(int)Define.StartIdx.PassPlusListStartIdx + i];

            AddItem(basicData.Reward_Item_Index, plusData.Reward_Item_Index, basicData.Index, plusData.Index, basicData.Level);
        }

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

    public void AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        Get<GameObject>((int)GameObjects.PassPanel).GetComponent<UI_PassPanel>().AddItem(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }
}
