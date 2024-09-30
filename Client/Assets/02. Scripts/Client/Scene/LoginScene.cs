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

        // 카메라 렌더 시 사용하는 카메라 설정
        // 블러 처리를 위한 데이터
        loginUI.GetComponent<Canvas>().worldCamera = Camera.main;

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

    }

    public override void Clear()
    {

    }
}
