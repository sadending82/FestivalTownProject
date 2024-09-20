using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Matching : UI_AlwaysOnTop
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
        Bind<GameObject>(typeof(GameObjects));
    }

}
