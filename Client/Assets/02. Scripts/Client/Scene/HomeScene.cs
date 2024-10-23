using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HomeScene : BaseScene
{
    UI_HomeScene homeSceneUI; 
    public bool isMatching = false;
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Home;

        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);

        Managers.UI.ShowSceneUI<UI_HomeStart>();
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
        homeSceneUI.SetNickName(Managers.Data.GetNickName());
    }

    public override void Clear()
    {
        
    }
}
