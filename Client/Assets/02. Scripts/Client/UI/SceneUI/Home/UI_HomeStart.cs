using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_HomeStart : UI_PopUp
{

    bool isMatching = false;

    enum GameObjects
    {
        Panel,
        GameStartButton,
        PresentButton,
        GalleryButton,
        SettingButton,
        ExitButton,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GameStartButton).BindEvent((PointerEventData) => { 
            Debug.Log($"게임 시작 버튼을 클릭했군요!");
            if (isMatching)
            {
                Managers.UI.ClosePopUpUI();
                Managers.Network.GetPacketManager().SendGameMatchingCancle();
                
            }
            else
            {
                Managers.UI.ShowPopUpUI<UI_Matching>();
                Managers.Network.GetPacketManager().SendGameMatchingRequest();
            }

            isMatching = !isMatching;
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => { Debug.Log($"선물 상자 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"갤러리 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"설정 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => { Debug.Log($"게임 종료 버튼을 클릭했군요!"); });
    }
}
