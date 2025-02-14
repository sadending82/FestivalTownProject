using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
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

    bool isInitialized = false;
    
    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Managers.Sound.Play("Sfx_Popup_In");

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
            {
                Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
            }
            Managers.UI.ClosePopUpUI();
            Managers.UI.ClosePopUpUI();
        });

        isInitialized = true;
    }

    public override void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (Managers.UI.GetTopOfPopUPUI() == this)
            {
                if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
                {
                    Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
                }
                Managers.UI.ClosePopUpUI();
                Managers.UI.ClosePopUpUI();
            }
        }
    }

    public void TextChange(string text)
    {
        Get<GameObject>((int)GameObjects.SignInSuccessText).GetComponent<TMP_Text>().text = text;
    }
}
