using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Loading : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        LoadingText,
    }


    void Start()
    {
        Init(); 
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));
    }
}
