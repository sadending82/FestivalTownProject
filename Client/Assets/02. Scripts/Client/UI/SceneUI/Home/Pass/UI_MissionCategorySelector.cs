using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_MissionCategorySelector : UI_Base
{
    enum GameObjects
    {
        DailyMissionButtonTab,
        PassMissionButtonTab,
    }

    enum Selected
    {
        DailyMission,
        PassMission,
    }

    bool isInitialized = false;
    Selected selected = Selected.DailyMission;
    UI_MissionPanel parentUI;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        foreach (GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_ButtonTab>().Init();
        }

        isInitialized = true;
    }

    public void SetSelected(int value)
    {
        selected = (Selected)value;
        SelectStuff();
    }

    public int GetSelected()
    {
        return (int)selected;
    }

    public void SetParentUI(UI_MissionPanel parentUI)
    {
        this.parentUI = parentUI;
        BindEventAfterSetParentUI();
    }


    public void BindEventAfterSetParentUI()
    {
        var btnUI = Get<GameObject>((int)GameObjects.DailyMissionButtonTab).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SelectCategory((int)GameObjects.DailyMissionButtonTab);
        });


        btnUI = Get<GameObject>((int)GameObjects.PassMissionButtonTab).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SelectCategory((int)GameObjects.PassMissionButtonTab);
        });

        selected = Selected.DailyMission;
        SelectStuff();
    }

    public void SelectStuff()
    {
        foreach (GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_ButtonTab>().SetSelected(false);
        }

        Get<GameObject>((int)selected).GetComponent<UI_ButtonTab>().SetSelected(true);
    }
}
