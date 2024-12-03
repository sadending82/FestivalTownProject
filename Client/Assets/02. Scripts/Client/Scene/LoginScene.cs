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

        // ī�޶� ���� �� ����ϴ� ī�޶� ����
        // �� ó���� ���� ������
        loginUI.GetComponent<Canvas>().worldCamera = Camera.main;

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (Managers.UI.GetTopOfPopUPUI() != null && Managers.UI.GetTopOfPopUPUI().GetComponent<UI_CreateAccount>() != null)
            {
                Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
                Managers.UI.ClosePopUpUI();
            }
        }

        if (Input.GetKeyDown(KeyCode.Return))
        {
            if (Managers.UI.GetTopOfPopUPUI() != null
                && Managers.UI.GetTopOfPopUPUI().GetComponent<UI_SignInSuccess>() != null)
            {
                // ���� ���� ���� �ִ� UI : ȸ������ ���� UI
                if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
                {
                    Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
                }
                Managers.UI.ClosePopUpUI();
                Managers.UI.ClosePopUpUI();
            }
            else if (Managers.UI.GetTopOfPopUPUI() != null &&
                Managers.UI.GetTopOfPopUPUI().GetComponent<UI_CreateAccount>() != null)
            {
                // ���� ���� ���� �ִ� UI : ȸ������ UI
                Managers.UI.GetTopOfPopUPUI().GetComponent<UI_CreateAccount>().CreateAccount();
            }
            else
            {
                Managers.UI.GetCurrentSceneUI().GetComponent<UI_Login>().Login();
            }
        }
    }

    public override void Clear()
    {

    }
}
