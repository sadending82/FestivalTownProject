using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_GachaCutScene : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        SkipButton,
    }

    bool isInitialized = false;
    int GachaType;
    int ResourceIndexToUse;

    void Start()
    {
        if (!isInitialized) Init();
    }
    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.SkipButton).BindEvent((PointerEventData) =>
        {
            SkipCutScene();
        }, Define.UIEvent.Click, true, true);

        isInitialized = true;
    }
    private void SkipCutScene()
    {
        SendGachaRequest();
    }


    public void SendGachaRequest()
    {
        Managers.Network.GetPacketManager().SendGachaRequestPacket(GachaType, ResourceIndexToUse);
    }

    public void SetGachaType(int value)
    {
        GachaType = value;
    }
    public void SetResourceIndexToUse(int index)
    {
        ResourceIndexToUse = index;
    }
}
