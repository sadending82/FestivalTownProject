using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_ShopClassifier : UI_Base
{
    enum GameObjects
    {
        ClassifierLabel,
        ClassifierTime,
        ClassifierCurrentCurrency,
    }

    bool isInitialzied = false;

    void Start()
    {
        if (!isInitialzied) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialzied = true;
    }

    public void SetCurrentCurrency(int amount)
    {
        Get<GameObject>((int)GameObjects.ClassifierCurrentCurrency).transform.GetChild(0).GetComponent<TMP_Text>().text = amount.ToString();
    }
}
