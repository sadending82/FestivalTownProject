using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Present : UI_PopUp
{

    enum GameObjects
    {
        Panel,
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
        TitleText,
        CharacterImage,
        BackButton,
        GachaSelectPanel,
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

        Get<GameObject>((int)GameObjects.BackButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(GetComponent<UI_Present>());
        }, Define.UIEvent.Click, true, true);

        isInitialized = true;
        
    }

    private void Update()
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = Managers.Data.GetGold().ToString();
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = Managers.Data.GetDiamond().ToString();
    }
}
