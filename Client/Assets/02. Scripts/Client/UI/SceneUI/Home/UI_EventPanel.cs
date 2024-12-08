using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_EventPanel : UI_Base
{

    enum GameObjects
    {
        TitlePanel,
        TogglePanel,
        AnnouncmentDataScrV,
        EventDataScrV,
    }

    bool isInitialized = false;

    int Selected = (int)Define.SelectedEventToggle.Event;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        var scrV = Get<GameObject>((int)GameObjects.EventDataScrV).GetComponent<UI_EventPanelDataScrV>();
        scrV.Init();

        foreach (var eventMain in Managers.Data.EventMainDataDict)
        {
            var ui = Managers.UI.MakeSubItem<UI_EventPanelData>(scrV.GetContent().transform);
            ui.Init();
            ui.SetAquireable(false);
            switch (eventMain.Key)
            {
                case 32001: // 출석 이벤트                
                    ui.BindEventofData(eventMain.Value.Name, (PointerEventData) =>
                    {
                        Managers.UI.ShowPopUpUI<UI_DailySignInCheck>();
                        ui.SetAquireable(false);
                        foreach (var aed in Managers.Data.AttendanceEventDataDict)
                        {
                            if(!aed.Value.isRewarded)
                            {
                                ui.SetAquireable(true);
                                break;
                            }
                        }
                    });
                    break;
                case 31001:
                    ui.BindEventofData(eventMain.Value.Name, (PointerEventData) =>
                    {
                        Debug.Log("디렉터의 감사 편지 인거 같아요");
                    });
                    break;
                default:
                    break;
            }
        }

        var toggleUI = Get<GameObject>((int)GameObjects.TogglePanel).GetComponent<UI_EventTogglePanel>();
        toggleUI.Init();
        toggleUI.SetParentUI(this);

        SetSelected((Define.SelectedEventToggle)Selected);

        isInitialized = true;
    }

    public void SetSelected(Define.SelectedEventToggle selected)
    {

        Selected = (int)selected;
        AllScrVOff();

        GameObject srcVObj = Get<GameObject>((int)GameObjects.AnnouncmentDataScrV + Selected);

        if (srcVObj != null)
        {
            srcVObj.SetActive(true);
        }
    }

    public void AllScrVOff()
    {

        Get<GameObject>((int)GameObjects.AnnouncmentDataScrV).SetActive(false);
        Get<GameObject>((int)GameObjects.EventDataScrV).SetActive(false);
    }
}
