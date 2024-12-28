using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_GachaResultScene : UI_Scene
{
    enum GameObjects
    {
        ItemName,
        Grade,
        OkButton,
        UI_MannequinModel
    }

    bool isInitialized = false;

    private int resultItemCode;
    private int acquiredItemCode;
    void Start()
    {
        if (!isInitialized) Init();
    }
    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.UI_MannequinModel).GetComponent<UI_MannequinModel>().Init();
        Get<GameObject>((int)GameObjects.UI_MannequinModel).GetComponent<UI_MannequinModel>().EquipResultItem(resultItemCode);

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_Present>();
            ui.Init();
        }, Define.UIEvent.Click, true, true);

        //등급 표시
        var resultItemData = Managers.Data.GetItemData(resultItemCode);
        Get<GameObject>((int)GameObjects.Grade).transform.GetChild(resultItemData.Item_Grade).gameObject.SetActive(true);

        Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().text = resultItemData.Name;

        //중복 아이템X
        if (resultItemCode == acquiredItemCode)
        {
            //New 표시
            Get<GameObject>((int)GameObjects.Grade).transform.GetChild(0).gameObject.SetActive(true);
        }
        //중복 아이템O
        {

        }

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        isInitialized = true;

        Camera.main.transform.GetChild(0).GetComponent<Camera>().fieldOfView = 20.0f;
    }
    public void SetResultItemCode(int resultItemCode)
    {
        this.resultItemCode = resultItemCode;
    }
    public void SetAcquiredItemCode(int acquiredItemCode)
    {
        this.acquiredItemCode = acquiredItemCode;
    }
}
