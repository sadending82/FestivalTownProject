using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_TopPanel : UI_Base
{
    enum GameObjects
    {
        HomeButton,
        CustomizeButton,
        ShopButton,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.HomeButton).BindEvent((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_HomeStart>() == null)
            {
                if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Customize>() != null)
                {
                    foreach (var itemPair in Managers.Data.ClientLocalCustomizingDataDict)
                    {
                        Managers.Data.PlayerCustomizingData[itemPair.Key] = itemPair.Value;
                    }

                    Managers.UI.CloseSceneUI();

                    Camera.main.transform.GetChild(0).gameObject.SetActive(true); // Home용 카메라
                    Camera.main.transform.GetChild(1).gameObject.SetActive(false); // Inventory용 카메라
                }

                Managers.UI.CloseSceneUI();

                var ui = Managers.UI.ShowSceneUI<UI_HomeStart>();
                ui.Init();
                ui.SetCustomizing();
            }
        });

        Get<GameObject>((int)GameObjects.CustomizeButton).BindEvent((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Customize>() == null)
            {
                Managers.Data.SetInventoryDataRecved(false);
                Managers.Network.GetPacketManager().SendUserItemsRequestPacket();
                StartCoroutine(WaitRecvItemDataAndShowUI());
            }    
        });

        Get<GameObject>((int)GameObjects.ShopButton).BindEvent((PointerEventData) =>
        {
            if (Managers.UI.GetCurrentSceneUI().GetComponent<UI_Shop>() == null)
            {
                Debug.Log("상점 이동");
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
        }

        if (timeOut < 0f)
        {
            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("인벤토리에 접속할 수 없습니다.");
            ui.BindPopupCloseEvent();
        }
    }
}
