using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_HomeStart : UI_PopUp
{

    bool isMatching = false;
    bool isPresentOpen = false;

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

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>().isMatching)
            {
                // ��Ī UI�� ������ ��Ŷ�� ���� ����
                // �ش� ��Ŷ�� ó�� �������� ������ ���̱� ������,
                // ���⼭ �������� �ʾƵ� �˴ϴ�.
                Managers.Network.GetPacketManager().SendGameMatchingCancle();
            }
            else
            {
                Managers.UI.ShowAOTUI<UI_Matching>();
                Managers.Network.GetPacketManager().SendGameMatchingRequest();

                // �̰ŵ� ���߿� �������� �ް� ó���ؾ� �� ������ ���Դϴ�.

                if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() != null)
                {
                    Managers.Scene.CurrentScene.GetComponent<HomeScene>().isMatching = true;
                }
            }
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Debug.Log($"���� ���� ��ư�� Ŭ���߱���!");

            Managers.UI.CloseAllPopUpUI();
            Managers.UI.ShowPopUpUI<UI_Present>();
            
        });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"������ ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"���� ��ư�� Ŭ���߱���!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => { Debug.Log($"���� ���� ��ư�� Ŭ���߱���!"); });
    }
}
