using System.IO;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_MatchingSelect: UI_PopUp
{
    enum GameObjects
    {
        Panel,
        IndivisualButton,
        TeamButton,
        WhatButton,
        TutorialButton,
        TestMatchingButton,
        ExitButton,
        MapCodeInputField,
    }

    // Start is called before the first frame update
    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        // 치트가 활성화 되어 있지 않을 때에는 없애주기
        if (false == Managers.Cheat.IsEnable())
        {
            Get<GameObject>((int)GameObjects.TestMatchingButton).SetActive(false);
            Get<GameObject>((int)GameObjects.MapCodeInputField).SetActive(false);
        }

        Get<GameObject>((int)GameObjects.IndivisualButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_SOLO);
            Managers.UI.ShowPopUpUI<UI_MatchingProgress>();
        });

        Get<GameObject>((int)GameObjects.TeamButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_TEAM);
            Managers.UI.ShowPopUpUI<UI_MatchingProgress>();
        });

        // 테스트용 매칭
        Get<GameObject>((int)GameObjects.TestMatchingButton).BindEvent((PointerEventData) =>
        {
            string str = "";
            if(Managers.Cheat.IsEnable())
            {
                str = Get<GameObject>((int)GameObjects.MapCodeInputField).GetComponent<TMP_InputField>().text;
            }


            string fileStr = Application.streamingAssetsPath + $"/Map/{str}.txt";

            if (File.Exists(fileStr))
            {
                Debug.Log("Map True");
                Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_TUTORIAL, int.Parse(str));
            }
            else
            {
                Debug.Log("Map False");
                Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_TUTORIAL, 10000);
            }
            Managers.UI.ShowPopUpUI<UI_MatchingProgress>();
        });

        Get<GameObject>((int)GameObjects.TutorialButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_TUTORIAL, 10000);        
            Managers.UI.ShowPopUpUI<UI_MatchingProgress>();
        });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            ClosePopUpUI();
        },
        Define.UIEvent.Click, true, true);


    }
}
