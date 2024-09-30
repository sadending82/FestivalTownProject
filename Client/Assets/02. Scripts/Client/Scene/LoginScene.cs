using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoginScene : BaseScene
{

    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Login;

        var loginUI = Managers.UI.ShowSceneUI<UI_Login>();

        loginUI.GetComponent<Canvas>().worldCamera = Camera.main;

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

    }

    public override void Clear()
    {

    }
}
