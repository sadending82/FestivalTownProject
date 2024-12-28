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

        UI_TopPanelButtonTab ui = Get<GameObject>((int)GameObjects.HomeButtonTab).GetComponent<UI_TopPanelButtonTab>();

        ui.Init();
        ui.SetEnable(true);
        ui.SetParentUI(this);
        ui.SetSelected(true);

        ui.BindEventToButton((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_HomeStart>() == null)
            {
                if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Customize>() != null)
                {
                    // �� �κп� �������� ������ Ŭ�� ���� �����ͷ� �����ϴ� �ڵ尡 ����־����ϴ�.
                    // �׷��� ������ ������� �����ϴ�.

                    Managers.UI.CloseSceneUI();

                    Camera.main.transform.GetChild(0).gameObject.SetActive(true); // Home�� ī�޶�
                    Camera.main.transform.GetChild(1).gameObject.SetActive(false); // Inventory�� ī�޶�
                }

                Managers.UI.CloseSceneUI();

                var hui = Managers.UI.ShowSceneUI<UI_HomeStart>();
                hui.Init();
                hui.SetCustomizing();

                selected = Selected.Home;
                SelectStuff();

            }
        });

        ui = Get<GameObject>((int)GameObjects.CustomizeButtonTab).GetComponent<UI_TopPanelButtonTab>();

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
                StartCoroutine(WaitRecvItemDataAndShowUI());
            }    
        });

        ui = Get<GameObject>((int)GameObjects.ShopButtonTab).GetComponent<UI_TopPanelButtonTab>();

        ui.Init();
        ui.SetEnable(false);
        ui.SetParentUI(this);
        ui.SetSelected(false);

        Get<GameObject>((int)GameObjects.ShopButtonTab).BindEvent((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Shop>() == null)
            {
                Debug.Log("���� �̵�");
            }
        });

        ui = Get<GameObject>((int)GameObjects.SettingButtonTab).GetComponent<UI_TopPanelButtonTab>();

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
            }
        });

        isInitialized = true;
    }

    IEnumerator WaitRecvItemDataAndShowUI()
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
            ui.NoticeTextChange("�κ��丮�� ������ �� �����ϴ�.");
            ui.BindPopupCloseEvent();
        }
    }

    public void SelectStuff()
    {
        foreach(GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_TopPanelButtonTab>().SetSelected(false);
        }

        Get<GameObject>((int)selected).GetComponent<UI_TopPanelButtonTab>().SetSelected(true);
    }
}
