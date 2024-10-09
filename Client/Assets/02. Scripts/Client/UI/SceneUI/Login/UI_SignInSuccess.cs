using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_SignInSuccess : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        SignInSuccessPanel,
        OkButton,
        SignInSuccessText,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
            {
                Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
            }
            Managers.UI.ClosePopUpUI();
            Managers.UI.ClosePopUpUI();
        });
    }
}
