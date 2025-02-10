using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_MissionCategory : UI_Base
{
    enum GameObjects
    {
        DailyMission,
        PassMission,
    }


    List<string> explainText = new() { "���� �̼��� ���� 00�ÿ� �ʱ�ȭ�˴ϴ�.", "�н� �̼��� �н� �Ⱓ���� 1ȸ Ŭ���� �����մϴ�." };

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

    public void SetCategoryOn(int idx)
    {
        foreach(var category in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)category).SetActive(false);
        }

        Get<GameObject>(idx).SetActive(true);

    }

    public Transform GetCategoryContent(int idx)
    {
        return Get<GameObject>(idx).transform.GetChild(0).GetChild(0).GetChild(0);
    }

}
