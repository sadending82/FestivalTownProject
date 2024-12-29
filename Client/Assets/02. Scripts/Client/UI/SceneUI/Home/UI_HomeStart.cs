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
        GameStartButton,
        PresentButton,
        UI_CharacterModel,
        AccountInitializeButton,
        GetMoneyButton,
        GetDiamondButton,
        EventButton,
        EventAquireableMark,
        AchivementButton,
        LockedButton,
        LockedButton2,
        PassButton,
        CommingSoonButton,
        RollingBanner,
    }

    bool isInitialized = false;

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

        Get<GameObject>((int)GameObjects.UI_CharacterModel).GetComponent<UI_CharacterModel>().Init();

        Get<GameObject>((int)GameObjects.GameStartButton).BindEvent((PointerEventData) => { 
            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            Managers.UI.ShowPopUpUI<UI_MatchingSelect>();
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
            Managers.UI.CloseAllPopUpUI();
            Managers.UI.CloseSceneUI();
            var ui = Managers.UI.ShowSceneUI<UI_Present>();
            ui.Init();
        });

        Get<GameObject>((int)GameObjects.AccountInitializeButton).BindEvent((PointerEventData) =>
        {
            // 계정 초기화 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
            Managers.Network.GetPacketManager().SendAccountResetPacket();
            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("계정 초기화 완료. 다시 로그인 해주십시오.");
            ui.BindGameEndEvent();

        });

        Get<GameObject>((int)GameObjects.GetMoneyButton).BindEvent((PointerEventData) =>
        {
            // 골드 획득 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
            Managers.Network.GetPacketManager().SendGetGoldCheatPacket();
        });

        Get<GameObject>((int)GameObjects.GetDiamondButton).BindEvent((PointerEventData) =>
        {
            // 다이아 획득 버튼에서 해야할 일
            // 패킷 보내기 정도 인듯?
            Managers.Network.GetPacketManager().SendGetDiaCheatPacket();
        });

        Get<GameObject>((int)GameObjects.EventButton).BindEvent((ButtonEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_Event>();
        });

        Get<GameObject>((int)GameObjects.EventAquireableMark).SetActive(false);

        if (Managers.Data.HasAttendanceDataRecved())
        {
            CheckAttendanceEventData();
        }

        if (false == Managers.Cheat.IsEnable())
        {
            Get<GameObject>((int)GameObjects.AccountInitializeButton).SetActive(false);
            Get<GameObject>((int)GameObjects.GetMoneyButton).SetActive(false);
            Get<GameObject>((int)GameObjects.GetDiamondButton).SetActive(false);
        }

        Camera.main.transform.GetChild(0).gameObject.SetActive(true);
        Camera.main.transform.GetChild(1).gameObject.SetActive(false);

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        isInitialized = true;
    }

    public void SetCustomizing()
    {
        Get<GameObject>((int)GameObjects.UI_CharacterModel).GetComponent<UI_CharacterModel>().SetCustomizing();
    }

    /// <summary>
    /// 인벤토리에서 아직 확정되지 않은 커스터마이징을 표시하기 위한 함수
    /// </summary>
    public void SetInventoryLocalCustomizing()
    {
        Get<GameObject>((int)GameObjects.UI_CharacterModel).GetComponent<UI_CharacterModel>().SetInventoryLocalCustomizing();
    }

    public void CheckAttendanceEventData()
    {
        Get<GameObject>((int)GameObjects.EventAquireableMark).SetActive(false);

        foreach (var aed in Managers.Data.AttendanceEventDataDict)
        {
            if (!aed.Value.isRewarded)
            {
                Get<GameObject>((int)GameObjects.EventAquireableMark).SetActive(true);
                break;
            }
        }
        
    }

}
