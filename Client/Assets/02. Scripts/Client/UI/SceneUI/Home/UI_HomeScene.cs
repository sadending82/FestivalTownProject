using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_HomeScene : UI_Scene
{
    enum GameObjects
    {
        Panel,
        PlayerImage,
        NickName,

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
