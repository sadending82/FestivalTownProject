using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Setting : UI_Scene
{
    enum GameObjects
    {
        UI_HomeBackground,
        UI_SettingPanel,
        ResetButton,
        SaveButton,
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

        Get<GameObject>((int)GameObjects.ResetButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.SaveButton).BindEvent((pointerEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_SettingSavePopUp>();
        });

        isInitialized = true;
    }



}
