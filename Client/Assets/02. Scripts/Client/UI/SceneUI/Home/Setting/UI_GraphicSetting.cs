using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GraphicSetting : UI_Base
{
    enum GameObjects
    {
        ResolutionSetting,
        DisplayModeSetting,
    }

    bool isInitialized = false;

    public void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        if (PlayerPrefs.HasKey("Resolution"))
        {
            var ui = Get<GameObject>((int)GameObjects.ResolutionSetting).GetComponent<UI_ResolutionSetting>();
            ui.Init();
            ui.setDropDown(PlayerPrefs.GetInt("Resolution"), false);
        }

        if (PlayerPrefs.HasKey("DisplayMode"))
        {
            var ui = Get<GameObject>((int)GameObjects.DisplayModeSetting).GetComponent<UI_DisplayModeSetting>();
            ui.Init();
            ui.setDropDown(PlayerPrefs.GetInt("DisplayMode"), false);
        }

        isInitialized = true;
    }
}
