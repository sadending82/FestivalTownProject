using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_CreateAccount : UI_PopUp
{

    enum GameObjects
    {
        Blocker,
        CreateAccountPanel,
        CreateAccountButton,
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
