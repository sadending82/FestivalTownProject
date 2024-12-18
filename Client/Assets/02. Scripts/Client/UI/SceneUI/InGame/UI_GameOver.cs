using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GameOver : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        GameOverImage,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

}
