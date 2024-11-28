using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

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
        if (false == Managers.Cheat.IsEnable()) Get<GameObject>((int)GameObjects.TestMatchingButton).SetActive(false);

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
            Managers.Network.GetPacketManager().SendGameMatchingRequest(NetworkProtocol.eMatchingType.FITH_TEST);
            Managers.UI.ShowPopUpUI<UI_MatchingProgress>();
        });

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            ClosePopUpUI();
        });
    }
}
