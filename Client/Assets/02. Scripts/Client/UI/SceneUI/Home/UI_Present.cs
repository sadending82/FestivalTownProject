using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Present : UI_Scene
{

    enum GameObjects
    {
        BG,
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
            ReturnToHomeScene();
        }, Define.UIEvent.Click, true, true);

        isInitialized = true;
    }

    void ReturnToHomeScene()
    {
        Managers.UI.CloseSceneUI();
        var ui = Managers.UI.ShowSceneUI<UI_HomeStart>();
        var popUpUi = Managers.UI.ShowPopUpUI<UI_HomeScene>();
        ui.Init();
        ui.SetCustomizing();
        popUpUi.Init();
    }

    private void Update()
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = Managers.Data.GetGold().ToString();
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = Managers.Data.GetDiamond().ToString();

        if(Input.GetKeyDown(KeyCode.Escape))
        {
            if(Managers.UI.GetTopOfPopUPUI() == null)
            {
                ReturnToHomeScene();
            }
        }
    }
}