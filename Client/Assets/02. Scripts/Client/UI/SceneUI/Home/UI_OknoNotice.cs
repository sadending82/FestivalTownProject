using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_OknoNotice : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        Panel,
        NoticeText,
        OkButton,
        NoButton,
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

        isInitialized = true;
    }

    public void BindEventToOkButton(Action<PointerEventData> action)
    {
        Get<GameObject>((int)GameObjects.OkButton).BindEvent(action);
    }

    public void BindPopupCloseEventToNoButton()
    {
        Get<GameObject>((int)GameObjects.NoButton).BindEvent((PointerEventData) =>
        {
            ClosePopUpUI();
        });
    }

    public void ChangeText(string str)
    {
        Get<GameObject>((int)GameObjects.NoticeText).GetComponent<TMP_Text>().text = str;
    }
}
