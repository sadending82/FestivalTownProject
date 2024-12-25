using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_ItemInfo : UI_Base
{
    enum GameObjects
    {
        Image,
        Name,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;
        transform.localPosition = Vector3.zero;

        isInitialized = true;
    }

    public void SetImage(string ImageName)
    {

    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.Name).GetComponent<TMP_Text>().text = name;
    }
}
