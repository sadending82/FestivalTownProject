using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Setting : UI_Scene
{
    enum GameObjects
    {

    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        isInitialized = true;
    }

}
