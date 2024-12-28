using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_SettingPanel : UI_Base
{
    enum GameObjects
    {
        SettingButtonTabs,
        SettingDetails,
    }

    bool isInitialized = false;
    int Selected = 0;

    private void Start()
    {
        if (!isInitialized) Init();        
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));



        isInitialized = true;
    }
}
