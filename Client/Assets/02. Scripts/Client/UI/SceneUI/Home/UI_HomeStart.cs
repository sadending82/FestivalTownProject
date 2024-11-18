using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_HomeStart : UI_Scene
{
    bool isMatching = false;
    bool isPresentOpen = false;

    enum GameObjects
    {
        BGBase,
        BGPattern,
        Title,
        GameStartButton,
        PresentButton,
        GalleryButton,
        SettingButton,
        ExitButton,
        CharacterImage,
        PlayerDataPanel,
        PlayerImage,
        NickName,
        ShopButton,
        PassButton,
        AchieveButton,
        InventoryButton,

    }

    bool isInitialized = false;

    bool isShopOpen = false;
    bool isPassOpen = false;
    bool isAchieveOpen = false;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GameStartButton).BindEvent((PointerEventData) => { 
            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            Managers.UI.ShowPopUpUI<UI_MatchingSelect>();
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
            Managers.UI.ShowPopUpUI<UI_Present>();
        });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"������ ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"���� ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
        });


        Get<GameObject>((int)GameObjects.ShopButton).BindEvent((PointerEventData) =>
        {
            if (isShopOpen)
            {
                Managers.UI.ClosePopUpUI();
            }
            else
            {
                Managers.UI.ShowPopUpUI<UI_Shop>();
            }

            isShopOpen = !isShopOpen;
        });

        Get<GameObject>((int)GameObjects.PassButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.AchieveButton).BindEvent((PointerEventData) =>
        {

        });

        Get<GameObject>((int)GameObjects.InventoryButton).BindEvent((PointerEventData) =>
        {
            Managers.Data.SetInventoryDataRecved(false);
            Managers.Network.GetPacketManager().SendUserItemsRequestPacket();
            StartCoroutine(WaitRecvItemDataAndShowUI());

        });

        isInitialized = true;
    }

    public void SetNickName(string nickName)
    {
        Get<GameObject>((int)GameObjects.NickName).GetComponent<TMP_Text>().text = nickName;
    }

    IEnumerator WaitRecvItemDataAndShowUI()
    {
        yield return null;
        while(!Managers.Data.IsInventoryDataRecved())
        {
            if(Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null)
            {
                break;
            }
            yield return null;
        }

        if(Managers.Data.IsInventoryDataRecved())
        {
            var ui = Managers.UI.ShowPopUpUI<UI_Inventory>();
            ui.Init();
        }
    }
}
