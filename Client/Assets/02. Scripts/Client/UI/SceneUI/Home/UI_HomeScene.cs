using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_HomeScene : UI_AlwaysOnTop
{
    enum GameObjects
    {
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetDiamond(int value)
    {
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public void SetGold(int value)
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = value.ToString();
    }

    private void Update()
    {
        
    }
}
