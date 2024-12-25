using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;

public class UI_GachaPopup : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        Image,
        Text,
        OkButton,
        NoButton,
    }

    int GachaType;
    int ResourceIndexToUse;
    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized)
        {         
            Init();
        }

    }

    public void SetGachaType(int value)
    {
        GachaType = value;
    }

    public void SetResourceIndexToUse(int index)
    {
        ResourceIndexToUse = index;
    }

    public void SetText(string str)
    {
        if(Get<GameObject>((int)GameObjects.Text) == null) return;
        Get<GameObject>((int)GameObjects.Text).GetComponent<TMP_Text>().text = str;
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            Debug.Log($"{GachaType} ±×·ì");
            Managers.UI.CloseSceneUI();
            var gachaCutScene = Managers.UI.ShowSceneUI<UI_GachaCutScene>();
            gachaCutScene.SetGachaType(GachaType);
            gachaCutScene.SetResourceIndexToUse(ResourceIndexToUse);
            gachaCutScene.Init();
            Managers.UI.ClosePopUpUI(this);
        });

        Get<GameObject>((int)GameObjects.NoButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this); 
        },
        Define.UIEvent.Click, true, true);

        isInitialized = true;
    }
}
