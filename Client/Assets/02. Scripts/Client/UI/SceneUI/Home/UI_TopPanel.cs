using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_TopPanel : UI_Base
{
    enum GameObjects
    {
        HomeButtonTab,
        CustomizeButtonTab,
        ShopButtonTab,
        SettingButtonTab,
    }

    enum Selected
    {
        Home,
        Customize,
        Shop,   
        Setting,
    }

    bool isInitialized = false;
    Selected selected = Selected.Home;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        selected = Selected.Home;

        UI_ButtonTab ui = Get<GameObject>((int)GameObjects.HomeButtonTab).GetComponent<UI_ButtonTab>();

        ui.Init();
        ui.SetEnable(true);
        ui.SetParentUI(this);
        ui.SetSelected(true);

        ui.BindEventToButton((PointerEventData) =>
        {
            ReturnToHomeScene();
        });

        ui = Get<GameObject>((int)GameObjects.CustomizeButtonTab).GetComponent<UI_ButtonTab>();

        ui.Init();
        ui.SetEnable(true);
        ui.SetParentUI(this);
        ui.SetSelected(false);

        ui.BindEventToButton((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Customize>() == null)
            {
                Managers.Data.SetInventoryDataRecved(false);
                Managers.Network.GetPacketManager().SendUserItemsRequestPacket();
                StartCoroutine(WaitRecvItemDataAndShowCustomUI());
            }    
        });

        ui = Get<GameObject>((int)GameObjects.ShopButtonTab).GetComponent<UI_ButtonTab>();

        ui.Init();
        ui.SetEnable(true);
        ui.SetParentUI(this);
        ui.SetSelected(false);

        ui.BindEventToButton((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Shop>() == null)
            {
                Managers.Data.SetInventoryDataRecved(false);
                Managers.Network.GetPacketManager().SendUserItemsRequestPacket();
                StartCoroutine(WaitRecvItemDataAndShowShopUI());
            }
        });

        ui = Get<GameObject>((int)GameObjects.SettingButtonTab).GetComponent<UI_ButtonTab>();

        ui.Init();
        ui.SetEnable(true);
        ui.SetParentUI(this);
        ui.SetSelected(false);

        ui.BindEventToButton((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Setting>() == null)
            {
                Managers.UI.CloseSceneUI();
                Managers.UI.ShowSceneUI<UI_Setting>();

                selected = Selected.Setting;
                SelectStuff();
            }
        });

        isInitialized = true;
    }

    IEnumerator WaitRecvItemDataAndShowCustomUI()
    {
        yield return null;

        float timeOut = 5.0f;

        while (!Managers.Data.IsInventoryDataRecved())
        {
            if (timeOut < 0f)
            {
                break;
            }

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null)
            {
                break;
            }

            timeOut -= Time.deltaTime;
            yield return null;
        }

        if (Managers.Data.IsInventoryDataRecved())
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_Customize>();
            ui.Init();

            selected = Selected.Customize;
            SelectStuff();

        }

        if (timeOut < 0f)
        {
            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("인벤토리에 접속할 수 없습니다.");
            ui.BindPopupCloseEvent();
        }
    }

    IEnumerator WaitRecvItemDataAndShowShopUI()
    {
        yield return null;

        float timeOut = 5.0f;

        while (!Managers.Data.IsInventoryDataRecved())
        {
            if (timeOut < 0f)
            {
                break;
            }

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null)
            {
                break;
            }

            timeOut -= Time.deltaTime;
            yield return null;
        }

        if (Managers.Data.IsInventoryDataRecved())
        {
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_Shop>();
            ui.Init();

            selected = Selected.Shop;
            SelectStuff();

        }

        if (timeOut < 0f)
        {
            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("상점에 접속할 수 없습니다.");
            ui.BindPopupCloseEvent();
        }
    }

    public void SelectStuff()
    {
        foreach(GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_ButtonTab>().SetSelected(false);
        }

        Get<GameObject>((int)selected).GetComponent<UI_ButtonTab>().SetSelected(true);
    }

    void ReturnToHomeScene()
    {
        if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_HomeStart>() == null)
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Customize>() != null)
            {
                // 이 부분에 기존에는 강제로 클라 로컬 데이터로 변경하는 코드가 들어있었습니다.
                // 그래서 문제가 생겼던것 같습니다.

                Managers.UI.CloseSceneUI();

                Camera.main.transform.GetChild(0).gameObject.SetActive(true); // Home용 카메라
                Camera.main.transform.GetChild(1).gameObject.SetActive(false); // Inventory용 카메라
            }

            Managers.UI.CloseSceneUI();

            var hui = Managers.UI.ShowSceneUI<UI_HomeStart>();
            hui.Init();
            hui.SetCustomizing();

            selected = Selected.Home;
            SelectStuff();

        }
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Escape))
        {
            if(Managers.UI.GetTopOfPopUPUI().GetComponent<UI_HomeScene>() != null)
            {
                ReturnToHomeScene();
            }
        }
    }
}
