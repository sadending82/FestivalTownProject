using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class UI_MissionData : UI_Base
{
    enum GameObjects
    {
        RecvButton,
        ExpData,
        MissionTitle,
        MissionDescription,
        RewardPanel,
    }

    bool isInitialized = false;
    string baseTitle = "";
    int countedTimes = 0;
    int required = 0;

    void Start()
    {
        transform.localScale = Vector3.one;

        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetMissionData(string title, string description, int exp, int rewardIdx, int rewardAmount, Action<PointerEventData> action)
    {
        SetExp(exp);
        SetTitle(title);
        SetDescription(description);
        SetReward(rewardIdx, rewardAmount);
        BindRecvButton(action);
    }

    public void BindRecvButton(Action<PointerEventData> action)
    {
        Get<GameObject>((int)GameObjects.RecvButton).BindEvent(action);
    }

    public void SetExp(int exp)
    {
        Get<GameObject>((int)GameObjects.ExpData).transform.GetChild(1).GetComponent<TMP_Text>().text = $"+{exp}p";
    }

    public void SetTitle(string title)
    {
        baseTitle = title;
        Get<GameObject>((int)GameObjects.MissionTitle).GetComponent<TMP_Text>().text = title + $" ({countedTimes} / {required})";
    }

    public void SetDescription(string descr)
    {
        Get<GameObject>((int)GameObjects.MissionDescription).GetComponent<TMP_Text>().text = descr;
    }

    public void SetReward(int rewardIdx, int amount)
    {
        var ImageTr = Get<GameObject>((int)GameObjects.RewardPanel).transform.GetChild(0);
        var TextTr = Get<GameObject>((int)GameObjects.RewardPanel).transform.GetChild(1);

        Managers.Data.ItemDict.TryGetValue(rewardIdx, out var itemData);

        ImageTr.GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
        TextTr.GetComponent<TMP_Text>().text = $"+{amount}";
    }

    public void SetMissionCounted(int counted, int require)
    {
        countedTimes = counted;
        required = require;

        SetTitle(baseTitle);
    }
}
