using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_HomeScene : UI_Scene
{
    enum GameObjects
    {
        Panel,
        PlayerImage,
        NickName,
        ShopButton,
        PassButton,
        AchieveButton,
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
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ShopButton).BindEvent((PointerEventData)=>
        {
            if(isShopOpen)
            {
                Managers.UI.CloseAllPopUpUI();
                Managers.UI.ShowPopUpUI<UI_HomeStart>();
            }
            else
            {
                Managers.UI.CloseAllPopUpUI();
                Managers.UI.ShowPopUpUI<UI_Shop>();
            }

            isShopOpen = !isShopOpen;
            isPassOpen = false;
            isAchieveOpen = false;
        });

        Get<GameObject>((int)GameObjects.PassButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.AchieveButton).BindEvent((PointerEventData) =>
        {

        });


    }


}
