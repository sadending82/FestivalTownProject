using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Matching : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        MatchingText
    }

    // Start is called before the first frame update
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
