using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Login : UI_Scene
{
    enum GameObjects
    {
        BackGroundImage,
        TitleLogo,
        LoginPanel,
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