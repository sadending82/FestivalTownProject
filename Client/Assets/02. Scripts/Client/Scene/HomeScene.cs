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
        homeSceneUI = Managers.UI.ShowAOTUI<UI_HomeScene>();

        StartCoroutine(SetDatas());

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    IEnumerator SetDatas()
    {
        yield return null;
        homeSceneUI.SetDiamond(Managers.Data.GetDiamond());
        homeSceneUI.SetGold(Managers.Data.GetGold());
        homeStartUI.SetNickName(Managers.Data.GetNickName());
    }

    public override void Clear()
    {
        
    }
}
