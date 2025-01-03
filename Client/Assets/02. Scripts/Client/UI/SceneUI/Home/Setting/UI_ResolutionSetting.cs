using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_ResolutionSetting : UI_Base
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

        switch (dropDown.value)
        {
            case 0:
                {
                    Resolution[] resolutions = Screen.resolutions;

                    bool isAbleToChange = false;

                    foreach (Resolution res in resolutions)
                    {
                        if (res.width == 1920 && res.height == 1080)
                        {
                            isAbleToChange = true;
                            break;
                        }
                    }

                    if (!isAbleToChange) return;
                    Screen.SetResolution(1920, 1080, Screen.fullScreenMode);
                }
                break;
            case 1:
                {
                    Resolution[] resolutions = Screen.resolutions;

                    bool isAbleToChange = false;

                    foreach (Resolution res in resolutions)
                    {
                        if (res.width == 1280 && res.height == 720)
                        {
                            isAbleToChange = true;
                            break;
                        }
                    }

                    if (!isAbleToChange) return;
                    Screen.SetResolution(1280, 720, Screen.fullScreenMode);
                }
                break;
        }

        PlayerPrefs.SetInt("Resolution", value);
    }
}
