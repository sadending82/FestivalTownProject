using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_CustomizeItemList : UI_Base
{
    enum GameObjects
    {
        CategoryButtons,
        CharacterCustomizeScrV,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public GameObject GetScrollView()
    {
        if (!isInitialized) Init();
        return Get<GameObject>((int)GameObjects.CharacterCustomizeScrV);
    }

}
