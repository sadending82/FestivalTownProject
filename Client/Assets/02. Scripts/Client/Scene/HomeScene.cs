using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HomeScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Home;

        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);

        Managers.UI.ShowSceneUI<UI_HomeScene>();
        Managers.UI.ShowPopUpUI<UI_HomeStart>();

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    public override void Clear()
    {
        
    }
}
