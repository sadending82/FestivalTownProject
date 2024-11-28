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


        // ġƮ�� Ȱ��ȭ �Ǿ� ���� ���� ������ �����ֱ�
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


        // �׽�Ʈ�� ��Ī
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
