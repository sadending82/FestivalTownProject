using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_HomeStart : UI_Scene
{
    bool isMatching = false;
    bool isPresentOpen = false;

    enum GameObjects
    {
        Panel,
        Title,
        GameStartButton,
        PresentButton,
        GalleryButton,
        SettingButton,
        ExitButton,
        CharacterImage,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GameStartButton).BindEvent((PointerEventData) => { 
            //Debug.Log($"���� ���� ��ư�� Ŭ���߱���!");

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            Managers.UI.ShowPopUpUI<UI_MatchingSelect>();
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Debug.Log($"���� ���� ��ư�� Ŭ���߱���!");

            Managers.UI.ShowPopUpUI<UI_Present>();
            
        });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"������ ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"���� ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => { Debug.Log($"���� ���� ��ư�� Ŭ���߱���!"); });
    }
}
