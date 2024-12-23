using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_GachaCutScene : UI_Scene
{
    enum GameObjects
    {
        Panel,
        SkipButton,
        UI_GachaponModel,
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

        Get<GameObject>((int)GameObjects.UI_GachaponModel).GetComponent<UI_GachaponModel>().Init();
        Get<GameObject>((int)GameObjects.UI_GachaponModel).GetComponent<UI_GachaponModel>().GachaponCustomizing(GachaType);

        Get<GameObject>((int)GameObjects.SkipButton).BindEvent((PointerEventData) =>
        {
            SkipCutScene();
        }, Define.UIEvent.Click, true, true);

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

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
