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

        // home에서 시작했을 때 1프레임 동안 개 크게 들리는 경우가 있어 코루틴으로 밀어두기
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
