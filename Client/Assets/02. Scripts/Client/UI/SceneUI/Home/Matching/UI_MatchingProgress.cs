using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_MatchingProgress : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        BG,
        TimerPanel,
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
