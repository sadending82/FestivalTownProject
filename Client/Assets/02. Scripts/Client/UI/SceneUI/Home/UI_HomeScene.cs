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

            }
            else
            {

            }
        });

        Get<GameObject>((int)GameObjects.PassButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.AchieveButton).BindEvent((PointerEventData) =>
        {

        });


    }


}
