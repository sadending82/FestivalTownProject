using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_HomeScene : UI_AlwaysOnTop
{
    enum GameObjects
    {
        Panel,
        PlayerImage,
        NickName,
        ShopButton,
        PassButton,
        AchieveButton,
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
    }

    bool isShopOpen = false;
    bool isPassOpen = false;
    bool isAchieveOpen = false;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ShopButton).BindEvent((PointerEventData)=>
        {
            if(isShopOpen)
            {
                Managers.UI.ClosePopUpUI();
            }
            else
            {
                Managers.UI.ShowPopUpUI<UI_Shop>();
            }

            isShopOpen = !isShopOpen;
        });

        Get<GameObject>((int)GameObjects.PassButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.AchieveButton).BindEvent((PointerEventData) =>
        {

        });

    }

    public void SetDiamond(int value)
    {
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public void SetGold(int value)
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = value.ToString();
    }
}
