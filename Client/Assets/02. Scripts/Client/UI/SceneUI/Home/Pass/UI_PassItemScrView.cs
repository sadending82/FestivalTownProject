using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_PassItemScrView : UI_Base
{
    enum GameObjects
    {
        Viewport,
        ScrollbarHorizontal
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        

        isInitialized = true;
    }

    public void AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        var ui = Managers.UI.MakeSubItem<UI_PassLevelItem>(Get<GameObject>((int)GameObjects.Viewport).transform.GetChild(0));
        ui.Init();

        ui.SetData(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }

}
