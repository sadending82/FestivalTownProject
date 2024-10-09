using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Point : UI_Base
{
    enum GameObjects
    {
        Green,
    }

    public bool isOn = true;

    void Start()
    {
        Init(); 
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetState(bool state)
    {
        if(state)
        {
            isOn = true;
            Get<GameObject>((int)GameObjects.Green).gameObject.SetActive(true);
        }
        else
        {
            isOn = false;
            Get<GameObject>((int)GameObjects.Green).gameObject.SetActive(false);
        }
    }
}
