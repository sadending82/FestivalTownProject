using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_PassItemPanel : UI_Base
{
    enum GameObjects
    {
        LabelImages,
        ItemScrView,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        var ui = Get<GameObject>((int)GameObjects.ItemScrView).GetComponent<UI_PassItemScrView>();
        ui.Init();

        isInitialized = true;
    }

    public UI_PassLevelItem AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        return Get<GameObject>((int)GameObjects.ItemScrView).GetComponent<UI_PassItemScrView>().AddItem(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }

}
