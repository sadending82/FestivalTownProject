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
            Debug.Log($"���� ���� ��ư�� Ŭ���߱���!");
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

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => { Debug.Log($"���� ���� ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"������ ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"���� ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => { Debug.Log($"���� ���� ��ư�� Ŭ���߱���!"); });
    }
}
