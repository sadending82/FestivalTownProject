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
        UI_CharacterModel,
        PlayerDataPanel,
        PlayerImage,
        NickName,
        ShopButton,
        PassButton,
        AchieveButton,
        InventoryButton,
        AccountInitializeButton,
        GetMoneyButton,
        GetDiamondButton,
    }

    bool isInitialized = false;

    bool isShopOpen = false;

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

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"갤러리 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"설정 버튼을 클릭했군요!"); });

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

        Get<GameObject>((int)GameObjects.AccountInitializeButton).BindEvent((PointerEventData) =>
        {
            // 계정 초기화 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
        });

        Get<GameObject>((int)GameObjects.GetMoneyButton).BindEvent((PointerEventData) =>
        {
            // 골드 획득 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
        });

        Get<GameObject>((int)GameObjects.GetDiamondButton).BindEvent((PointerEventData) =>
        {
            // 다이아 획득 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
        });

        if (false == Managers.Cheat.IsEnable())
        {
            Get<GameObject>((int)GameObjects.AccountInitializeButton).SetActive(false);
            Get<GameObject>((int)GameObjects.GetMoneyButton).SetActive(false);
            Get<GameObject>((int)GameObjects.GetDiamondButton).SetActive(false);
        }

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;


        isInitialized = true;
    }

    public void SetNickName(string nickName)
    {
        Get<GameObject>((int)GameObjects.NickName).GetComponent<TMP_Text>().text = nickName;
    }

    public void SetCustomizing()
    {
        Get<GameObject>((int)GameObjects.UI_CharacterModel).GetComponent<UI_CharacterModel>().SetCustomizing();
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
