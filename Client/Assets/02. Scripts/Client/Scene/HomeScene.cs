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

        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);

        homeStartUI = Managers.UI.ShowSceneUI<UI_HomeStart>();
        homeSceneUI = Managers.UI.ShowPopUpUI<UI_HomeScene>();

        homeStartUI.Init();
        homeSceneUI.Init();

        if (!Managers.Data.HasAttendanceUIPopUp())
        {
            Managers.Data.SetAttendanceUIPopUp(true);
            StartCoroutine(WaitAttendanceEventData());
        }

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    IEnumerator WaitAttendanceEventData()
    {
        yield return null;
        
        while(!Managers.Data.HasAttendanceDataRecved())
        {
            yield return null;
        }

        Managers.UI.ShowPopUpUI<UI_DailySignInCheck>();
    }

    public override void Clear()
    {
        
    }
}
