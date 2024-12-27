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

    private float camStartFOV;
    private const float camZoomInFOV = 7.0f;
    private float zoomInPercent;
    private bool isZoomin = false;

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

        StartCutScene();
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
    private void StartCutScene()
    {
        ZoomIn();
    }
    private void ZoomIn()
    {
        camStartFOV = Camera.main.transform.GetChild(0).GetComponent<Camera>().fieldOfView;
        zoomInPercent = 0;
        isZoomin = true;
    }
    private void FixedUpdate()
    {
        if (isZoomin)
        {
            zoomInPercent += Time.deltaTime * 1.2f;
            if (zoomInPercent > 1)
            {
                zoomInPercent = 1;
                isZoomin = false;
                Get<GameObject>((int)GameObjects.UI_GachaponModel).GetComponent<UI_GachaponModel>().GachaStart();
            }
            Camera.main.transform.GetChild(0).GetComponent<Camera>().fieldOfView = Mathf.Lerp(camStartFOV, camZoomInFOV, zoomInPercent);
        }
    }

}
