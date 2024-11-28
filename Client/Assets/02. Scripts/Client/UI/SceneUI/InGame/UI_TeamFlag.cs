using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_TeamFlag : UI_Base
{
    enum GameObjects
    {
        RedWin,
        BlueWin,
        GreenWin,
        RedLose,
        BlueLose,
        GreenLose,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        AllFlagOff();

        isInitialized = true;
    }

    public void AllFlagOff()
    {
        foreach(GameObjects gos in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)gos).SetActive(false);
        }
    }

    public void SetFlag(int team, bool isWin)
    {
        switch(team)
        {
            case 0: // Red
                if (isWin) Get<GameObject>((int)GameObjects.RedWin).SetActive(true);
                else Get<GameObject>((int)GameObjects.RedLose).SetActive(true);
                break;
            case 1: // blue
                if (isWin) Get<GameObject>((int)GameObjects.BlueWin).SetActive(true);
                else Get<GameObject>((int)GameObjects.BlueLose).SetActive(true);
                break;
            case 2: // green
                if (isWin) Get<GameObject>((int)GameObjects.GreenWin).SetActive(true);
                else Get<GameObject>((int)GameObjects.GreenLose).SetActive(true);
                break;
            default:
                break;
        }
    }


}
