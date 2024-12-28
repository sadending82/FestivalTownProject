using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.Rendering.DebugUI;

public class UI_GachaSelectPanel : UI_Base
{
    enum GameObjects
    {
        GachaTypes,
        LeftButton,
        RightButton,
    }

    int TypeCount = 0;
    public int CurrentSelected = 0;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.RightButton).BindEvent((PointerEventData) =>
        {
            SelectNextGacha();
        });

        Get<GameObject>((int)GameObjects.LeftButton).BindEvent((PointerEventData) =>
        {
            SelectPrevGacha();
        });

        foreach (var group in Managers.Data.GachaGroupDict)
        {
            var ui = Managers.UI.MakeSubItem<UI_GachaType>(Get<GameObject>((int)GameObjects.GachaTypes).transform);

            ui.SetGachaType(group.Value.Index);
            ui.Init();

            ui.SetRequireResource(group.Value.Pay_Item1_Index, group.Value.Pay_Item2_Index);

            TypeCount++;
        }

        SelectLastGachaType();
    }

    void AllGachaOff()
    {
        GameObject gt = Get<GameObject>((int)GameObjects.GachaTypes);
        UI_GachaType[] allChildren = gt.GetComponentsInChildren<UI_GachaType>();
        foreach(UI_GachaType child in allChildren)
        {
            if (child.name == gt.name)
                continue;

            child.gameObject.SetActive(false);
        }
    }

    void CurrentGachaOn()
    {
        AllGachaOff();
        Get<GameObject>((int)GameObjects.GachaTypes).transform.GetChild(CurrentSelected).gameObject.SetActive(true);
        Managers.UI.lastGachaType = CurrentSelected;
    }

    public void SelectNextGacha()
    {
        CurrentSelected++;
        if (CurrentSelected >= TypeCount) 
        {
            CurrentSelected = 0;
        }
        CurrentGachaOn();
    }

    public void SelectPrevGacha()
    {
        CurrentSelected--;
        if (CurrentSelected < 0)
        {
            CurrentSelected = TypeCount - 1;
        }
        CurrentGachaOn();
    }

    public void SelectLastGachaType()
    {
        CurrentSelected = Managers.UI.lastGachaType;
        CurrentGachaOn();
    }
}
