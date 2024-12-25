using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_EventTogglePanel : UI_Base
{
    enum GameObjects
    {
        AnnouncementButton,
        EventButton,
    }

    bool isInitialized = false;
    UI_EventPanel parentUI;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.AnnouncementButton).BindEvent((PointerEventData) =>
        {
            if (parentUI == null) return;

            parentUI.SetSelected(Define.SelectedEventToggle.Announcement);
        });

        Get<GameObject>((int)GameObjects.EventButton).BindEvent((PointerEventData) =>
        {
            if (parentUI == null) return;

            parentUI.SetSelected(Define.SelectedEventToggle.Event);
        });

        isInitialized = true;
    }

    public void SetParentUI(UI_EventPanel ui)
    {
        parentUI = ui;
    }

}
