using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_DailyCheckItem : UI_Base
{
    enum GameObjects
    {
        VerticalLayout,
        AquireableMark,
    }

    bool isInitialized = false;
    int eventCode = -1;
    int itemIndex = -1;
    bool itemRewarded = false;
    bool isAquireable = false;
    int day = -1;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().Init();
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetParentUI(this);

        transform.localScale = Vector3.one;

        SetItemRewarded(itemRewarded);
        SetAquireable(isAquireable);

        isInitialized = true;
    }

    public void SetEventCode(int code)
    {
        eventCode = code;
    }

    public int GetEventCode()
    {
        return eventCode;
    }

    public void SetDay(int value)
    {
        day = value;
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetDay(day);
    }

    public int GetDay()
    {
        return day;
    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetName(name);
    }

    public void SetItemRewarded(bool rewarded)
    {
        itemRewarded = rewarded;
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetItemRewarded(itemRewarded);
    }

    public bool IsItemRewarded()
    {
        return itemRewarded;
    }

    public void SetAquireable(bool value)
    {
        isAquireable = value;
        Get<GameObject>((int)GameObjects.AquireableMark).SetActive(isAquireable);
    }

    public bool IsAquireable()
    {
        return isAquireable;
    }

    public void SetItemIndex(int index)
    {
        itemIndex = index;
    }

    public int GetItemIndex()
    {
        return itemIndex; 
    }

    public void BindEvent(int dataIndex)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().BindEvent(dataIndex);
    }

}
