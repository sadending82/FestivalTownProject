using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_SettingPanel : UI_Base
{
    enum GameObjects
    {
        SettingButtonTabs,
        SettingDetails,
    }

    bool isInitialized = false;
    int Selected = 0;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        var sbUI = Get<GameObject>((int)GameObjects.SettingButtonTabs).GetComponent<UI_SettingTab>();
        sbUI.Init();

        sbUI.SetParentUI(this);


        var sdUI = Get<GameObject>((int)GameObjects.SettingDetails).GetComponent<UI_SettingDetails>();
        sdUI.Init();


        isInitialized = true;
    }

    public void SelectDetailUI(int idx)
    {
        var sbUI = Get<GameObject>((int)GameObjects.SettingButtonTabs).GetComponent<UI_SettingTab>();

        sbUI.SetSelected(idx);

        var sdUI = Get<GameObject>((int)GameObjects.SettingDetails).GetComponent<UI_SettingDetails>();

        sdUI.SetDetailOn(idx);
    }

    public void SetDefaults()
    {
        Get<GameObject>((int)GameObjects.SettingDetails).GetComponent<UI_SettingDetails>().SetDefaults();
    }
}
