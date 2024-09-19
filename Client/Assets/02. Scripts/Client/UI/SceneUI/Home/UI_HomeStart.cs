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
            Debug.Log($"게임 시작 버튼을 클릭했군요!");

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() == null) return;

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>().isMatching)
            {
                // 매칭 UI의 삭제는 패킷을 받은 이후
                // 해당 패킷의 처리 과정에서 지워줄 것이기 때문에,
                // 여기서 삭제하지 않아도 됩니다.
                Managers.Network.GetPacketManager().SendGameMatchingCancle();
            }
            else
            {
                Managers.UI.ShowAOTUI<UI_Matching>();
                Managers.Network.GetPacketManager().SendGameMatchingRequest();

                // 이거도 나중에 서버에서 받고 처리해야 할 것으로 보입니다.

                if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() != null)
                {
                    Managers.Scene.CurrentScene.GetComponent<HomeScene>().isMatching = true;
                }
            }
        });

        Get<GameObject>((int)GameObjects.PresentButton).BindEvent((PointerEventData) => {
            Debug.Log($"선물 상자 버튼을 클릭했군요!");

            Managers.UI.CloseAllPopUpUI();
            Managers.UI.ShowPopUpUI<UI_Present>();
            
        });

        Get<GameObject>((int)GameObjects.GalleryButton).BindEvent((PointerEventData) => { Debug.Log($"갤러리 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.SettingButton).BindEvent((PointerEventData) => { Debug.Log($"설정 버튼을 클릭했군요!"); });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) => { Debug.Log($"게임 종료 버튼을 클릭했군요!"); });
    }
}
