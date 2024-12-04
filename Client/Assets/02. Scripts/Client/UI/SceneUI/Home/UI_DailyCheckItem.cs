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

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().Init();

        transform.localScale = Vector3.one;

        SetItemRewarded(false);
        SetAquireable(false);

        isInitialized = true;
    }

    public void SetDay(int day)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetDay(day);
    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetName(name);
    }

    public void SetItemRewarded(bool rewarded)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().SetItemRewarded(rewarded);
    }

    public void SetAquireable(bool value)
    {
        Get<GameObject>((int)GameObjects.AquireableMark).SetActive(value);
    }

    public void BindEvent(int dataIndex)
    {
        Get<GameObject>((int)GameObjects.VerticalLayout).GetComponent<UI_DailyCheckItemVertLayout>().BindEvent(dataIndex);
    }

}
