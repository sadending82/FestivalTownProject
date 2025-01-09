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
    private int acquiredItemAmount;

    void Start()
    {
        if (!isInitialized) Init();
    }
    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.UI_MannequinModel).GetComponent<UI_MannequinModel>().Init();

        //획득 아이템 마네킹에 착용
        Get<GameObject>((int)GameObjects.UI_MannequinModel).GetComponent<UI_MannequinModel>().EquipResultItem(resultItemCode);

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_Present>();
            ui.Init();
        }, Define.UIEvent.Click, true, true);

        var resultItemData = Managers.Data.GetItemData(resultItemCode);
        //이름 표시

        //중복 아이템X
        if (resultItemCode == acquiredItemCode)
        {
            //등급 표시
            Get<GameObject>((int)GameObjects.Grade).transform.GetChild(resultItemData.Item_Grade).gameObject.SetActive(true);

            if (resultItemData.Name == "골드")
            {
                string result = $"{resultItemData.Name} {acquiredItemAmount}개";
                Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().text = result;
                float textLength = Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().preferredWidth;
            }
            else
            {
                //New 표시
                Get<GameObject>((int)GameObjects.Grade).transform.GetChild(0).gameObject.SetActive(true);
                
                Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().text = resultItemData.Name;
                float textLength = Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().preferredWidth;
            }
        }
        //중복 아이템O
        else
        {
            string result = $"중복 아이템 마일리지 {acquiredItemAmount}개";
            Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().text = result;
            float textLength = Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().preferredWidth;
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
    public void SetAcquiredItemAmount(int acquiredItemAmount)
    {
        this.acquiredItemAmount = acquiredItemAmount;
    }
}
