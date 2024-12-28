using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_GachaCutScene : UI_Scene
{
    enum GameObjects
    {
        SkipButton,
        UI_GachaponModel,
    }

    bool isInitialized = false;
    int GachaType;

    private float camStartFOV;
    private const float camZoomInFOV = 7.0f;
    private float zoomInPercent;
    private bool isZoomin = false;

    private int resultItemCode;
    private int acquiredItemCode;

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
        ShowGachaRequest();
    }

    public void ShowGachaRequest()
    {
        Managers.UI.CloseSceneUI();
        var ui = Managers.UI.ShowSceneUI<UI_GachaResultScene>();
        ui.SetResultItemCode(resultItemCode);
        ui.SetAcquiredItemCode(acquiredItemCode);
        ui.Init();
    }

    public void SetGachaType(int value)
    {
        GachaType = value;
    }
    private void StartCutScene()
    {
        StartCoroutine("ZoomIn");
    }
    IEnumerator ZoomIn()
    {
        yield return new WaitForSeconds(0.3f);
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

    public void SetResultItemCode(int resultItemCode)
    {
        this.resultItemCode = resultItemCode;
    }
    public void SetAcquiredItemCode(int acquiredItemCode)
    {
        this.acquiredItemCode = acquiredItemCode;
    }
    public int GetResultItemGrade()
    {
        var resultItemData = Managers.Data.GetItemData(resultItemCode);
        return resultItemData.Item_Grade;
    }
}