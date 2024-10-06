using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HomeScene : BaseScene
{

    public bool isMatching = false;
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Home;

        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);

        Managers.UI.ShowSceneUI<UI_HomeStart>();
        Managers.UI.ShowAOTUI<UI_HomeScene>();

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    public override void Clear()
    {
        
    }
}
