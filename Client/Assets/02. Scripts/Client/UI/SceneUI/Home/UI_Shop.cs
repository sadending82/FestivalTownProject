using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Shop : UI_PopUp
{

    enum GameObjects
    {
        Panel,
        Text
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }
}
