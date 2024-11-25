using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_DailySignInCheck : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        BG,
        ExitButton,
        ItemGrid
    }

    bool isInitialized = false;

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
            foreach (var eventData in Managers.Data.EventMainDataDict)
            {
                if (eventData.Value.Type == 2)
                {
                    Managers.Network.GetPacketManager().SendAttendanceEventRequestPacket(eventData.Value.Event_Id);
                }
            }
            Managers.UI.ClosePopUpUI();
        });

        GameObject itemGrid = Get<GameObject>((int)GameObjects.ItemGrid);

        foreach (var eventData in Managers.Data.EventMainDataDict)
        {
            if (eventData.Value.Type == 2)
            {
                int dataIndex = eventData.Key + 100000;
                while(Managers.Data.EventListDataDict.ContainsKey(dataIndex))
                {          
                    Managers.Data.EventListDataDict.TryGetValue(dataIndex, out var data);

                    var ui = Managers.UI.MakeSubItem<UI_DailyCheckItem>(itemGrid.transform);
                    ui.Init();
                    ui.SetDay(data.Day);
                    ui.SetName(Managers.Data.ItemDict[data.Reward_Item_Index].Name);
                    
                    bool result = Managers.Data.AttendanceEventDataDict.TryGetValue(data.Day, out var attendanceEventData);     
                    ui.SetItemRewarded(result);
                    ++dataIndex;
                }
            }
        }

        isInitialized = true;
    }
}
