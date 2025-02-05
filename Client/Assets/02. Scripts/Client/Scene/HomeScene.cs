using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HomeScene : BaseScene
{
    UI_HomeScene homeSceneUI;
    UI_HomeStart homeStartUI;
    public bool isMatching = false;
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Home;

        homeStartUI = Managers.UI.ShowSceneUI<UI_HomeStart>();
        homeSceneUI = Managers.UI.ShowPopUpUI<UI_HomeScene>();

        homeStartUI.Init();
        homeSceneUI.Init();

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

        Input.imeCompositionMode = IMECompositionMode.Off;
    }

    public void Start()
    {

        // home���� �������� �� 1������ ���� �� ũ�� �鸮�� ��찡 �־� �ڷ�ƾ���� �о�α�
        StartCoroutine(StartBGM());
    }

    IEnumerator StartBGM()
    {
        yield return null;
        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);
        yield break;
    }

    public override void Clear()
    {
        
    }
}
