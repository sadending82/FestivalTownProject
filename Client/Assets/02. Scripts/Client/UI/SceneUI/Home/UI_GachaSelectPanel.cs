using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GachaSelectPanel : UI_Base
{
    enum GameObjects
    {
        NormalGacha,
        UltraGacha,
    }

    public int CurrentSelected = 0;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        CurrentGachaOn();
    }

    void AllGachaOff()
    {
        Get<GameObject>((int)GameObjects.NormalGacha).SetActive(false);
        Get<GameObject>((int)GameObjects.UltraGacha).SetActive(false);
    }

    void CurrentGachaOn()
    {
        AllGachaOff();
        Get<GameObject>(CurrentSelected).SetActive(true);
    }

    public void SelectNextGacha()
    {
        if(CurrentSelected == 1) 
        {
            CurrentSelected = 0;
        }
        else
        {
            CurrentSelected++;
        }
        CurrentGachaOn();
    }

    public void SelectPrevGacha()
    {
        if (CurrentSelected == 0)
        {
            CurrentSelected = 1;
        }
        else
        {
            CurrentSelected--;
        }
        CurrentGachaOn();
    }
}
