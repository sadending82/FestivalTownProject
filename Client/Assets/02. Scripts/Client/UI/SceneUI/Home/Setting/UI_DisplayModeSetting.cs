using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_DisplayModeSetting : UI_Base
{
    enum GameObjects
    {
        Text,
        DropDown,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        TMP_Dropdown dropDown = Get<GameObject>((int)GameObjects.DropDown).GetComponent<TMP_Dropdown>();

        dropDown.onValueChanged.AddListener(delegate { setDropDown(dropDown.value, true); });

        isInitialized = true;
    }

    public void setDropDown(int value, bool actualChange)
    {
        TMP_Dropdown dropDown = Get<GameObject>((int)GameObjects.DropDown).GetComponent<TMP_Dropdown>();
        dropDown.value = value;

        if (!actualChange) return;

        switch (value)
        {
            case 0:
                Screen.SetResolution(Screen.currentResolution.width, Screen.currentResolution.height, FullScreenMode.Windowed);
                break;
            case 1:
                Screen.SetResolution(Screen.currentResolution.width, Screen.currentResolution.height, FullScreenMode.FullScreenWindow);
                break;
            case 2:
                Screen.SetResolution(Screen.currentResolution.width, Screen.currentResolution.height, FullScreenMode.FullScreenWindow);
                break;
        }

        PlayerPrefs.SetInt("DisplayMode", value);
        Debug.Log($"DisplayMode Changed to {value}");
    }
}
