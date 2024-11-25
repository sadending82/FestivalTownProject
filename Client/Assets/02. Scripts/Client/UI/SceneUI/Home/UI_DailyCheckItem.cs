using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_DailyCheckItem : UI_Base
{
    enum GameObjects
    {
        Day,
        ItemImage,
        Name,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;
        SetItemRewarded(false);
        isInitialized = true;
    }

    public void SetDay(int day)
    {
        Get<GameObject>((int)GameObjects.Day).GetComponent<TMP_Text>().text = $"Day {day}";
    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.Name).GetComponent<TMP_Text>().text = $"{name}";
    }

    public void SetItemRewarded(bool rewarded)
    {
        if(rewarded)
        {
            // 0번 => 아이템 이미지
            // 1번 => 받았는 지 체크용 이미지
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(true);
        }
        else
        {
            // 0번 => 아이템 이미지
            // 1번 => 받았는 지 체크용 이미지
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(false);
        }
    }
}
