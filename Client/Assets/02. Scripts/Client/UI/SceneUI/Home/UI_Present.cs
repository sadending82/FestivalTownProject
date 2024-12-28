using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Present : UI_Scene
{

    enum GameObjects
    {
        Panel,
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
        Title,
        CharacterImage,
        BackButton,
        GachaSelectPanel,
        MileageShop,
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

        Get<GameObject>((int)GameObjects.BackButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_HomeStart>();
            var popUpUi = Managers.UI.ShowPopUpUI<UI_HomeScene>();
            ui.Init();
            ui.SetCustomizing();
            popUpUi.Init();
        }, Define.UIEvent.Click, true, true);

        isInitialized = true;

        Camera.main.transform.GetChild(0).GetComponent<Camera>().fieldOfView = 20.0f;
    }

    private void Update()
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = Managers.Data.GetGold().ToString();
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = Managers.Data.GetDiamond().ToString();
    }
}