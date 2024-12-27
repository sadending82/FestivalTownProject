using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GachaSlider : UI_PopUp
{
    enum GameObjects
    {
        Slider,
        Text
    }

    bool isInitialized = false;
    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }
    public override void Init()
    {
        base.Init();
        Bind<GameObject>(typeof(GameObjects));
        isInitialized = true;
    }
}