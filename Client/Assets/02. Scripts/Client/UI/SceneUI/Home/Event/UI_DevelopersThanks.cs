using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_DevelopersThanks : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        Panel,
        Title,
        Message,
        ExitButton,
        GetDiaButton,
        Aquired,
    }

    bool isInitialized = false;
    int eventCode = 31001;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
            if (Managers.UI.GetTopOfPopUPUI().GetComponent<UI_Event>() != null)
            {
                Managers.UI.GetTopOfPopUPUI().GetComponent<UI_Event>().GetEventPanel().CheckEventData();
            }      
        }, Define.UIEvent.Click, true, true);

        Get<GameObject>((int)GameObjects.GetDiaButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendAttendanceRewardRequestPacket(eventCode, 1);
        });

        int eventListIdx = eventCode + 100000;

        if(Managers.Data.AttendanceEventDataDict.TryGetValue(eventListIdx, out var data))
        {
            if (data.isRewarded == false)
            {
                Get<GameObject>((int)GameObjects.Aquired).SetActive(false);
            }
        }

        isInitialized = true;
    }

    public void UpdateItemCheck()
    {
        Debug.Log("updateItemCheck");
        Get<GameObject>((int)GameObjects.Aquired).SetActive(true);

        int eventListIdx = eventCode + 100000;

        if (Managers.Data.AttendanceEventDataDict.TryGetValue(eventListIdx, out var data))
        {
            Debug.Log($"{eventListIdx}. isRewarded is {data.isRewarded}");
            if (data.isRewarded == false)
            {
                Get<GameObject>((int)GameObjects.Aquired).SetActive(false);
            }
        }
    }
}
