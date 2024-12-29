using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_SettingDetails : UI_Base
{
    enum GameObjects
    {
        SoundSetting,
        GraphicSetting,
        ControlSetting,
    }

    bool isInitialized = false;

    private void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        SetDetailOn(0);

        isInitialized = true;
    }

    public void SetDetailOn(int index)
    {
        foreach(GameObjects objIdx in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)objIdx).SetActive(false);
        }

        Get<GameObject>(index).SetActive(true);
    }
}
