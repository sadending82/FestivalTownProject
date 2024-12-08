using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_EventPanelData : UI_Base
{
    enum GameObjects
    {
        Text,
        EventAquireableMark,
    }

    bool isInitialzed = false;

    private void Start()
    {
        if (!isInitialzed) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;

        isInitialzed = true;
    }


    public void BindEventofData(string dataName, Action<PointerEventData> action)
    {
        if(!isInitialzed) Init();
        Get<GameObject>((int)GameObjects.Text).GetComponent<TMP_Text>().text = dataName;

        gameObject.BindEvent(action);
    }

    public void SetText(string text)
    {
        if(!isInitialzed) Init();

        Get<GameObject>((int)GameObjects.Text).GetComponent<TMP_Text>().text = text;
    }

    public void SetAquireable(bool value)
    {
        Get<GameObject>((int)GameObjects.EventAquireableMark).SetActive(value);
    }

}
