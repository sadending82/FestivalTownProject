using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Pass : UI_Scene
{
    enum GameObjects
    {
        BG,
        ExitButton,
        TitleText,
        BuyPassButton,
        PassPanel,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_HomeStart>();
            var popUpUi = Managers.UI.ShowPopUpUI<UI_HomeScene>();
            ui.Init();
            ui.SetCustomizing();
            popUpUi.Init();
        }, Define.UIEvent.Click, true, true);

        isInitialized = true;
    }
}
