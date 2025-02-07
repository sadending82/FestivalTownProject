using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Mission : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        MissionPanel,
        ExitButton,
        MissionExplainText
    }

    bool isInitialized = false;
    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

        isInitialized = true;
    }
}
