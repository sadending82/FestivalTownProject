using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Event : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        EventPanel,
        ExitButton,
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

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI();
        }, Define.UIEvent.Click, true, true);

        var eventPanelUI = Get<GameObject>((int)GameObjects.EventPanel).GetComponent<UI_EventPanel>();

        isInitialized = true;
    }
}
