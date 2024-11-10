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
    }

    bool isShopOpen = false;
    bool isPassOpen = false;
    bool isAchieveOpen = false;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GameStartButton).BindEvent((PointerEventData) => { 
            //Debug.Log($"게임 시작 버튼을 클릭했군요!");

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            Managers.UI.ShowPopUpUI<UI_MatchingSelect>();
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Debug.Log($"선물 상자 버튼을 클릭했군요!");

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
    }

    public void SetNickName(string nickName)
    {
        Get<GameObject>((int)GameObjects.NickName).GetComponent<TMP_Text>().text = nickName;
    }
}
