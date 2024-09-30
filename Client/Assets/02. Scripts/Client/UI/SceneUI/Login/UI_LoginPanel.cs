using SuperBlur;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_LoginPanel: UI_Base
{
    enum GameObjects
    {
        IDInputField,
        IDImage,
        PWInputField,
        PWImage,
        CreateAccountButton,
        FindIDButton,
        FindPWButton,
        NoticeButton,
        ButtonTextSpacer,
        LoginButton,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.CreateAccountButton).BindEvent((PointerEventData) => {
            Debug.Log("회원가입 버튼 클릭");
            Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = true;
            Managers.UI.ShowPopUpUI<UI_CreateAccount>();
        });

        Get<GameObject>((int)GameObjects.LoginButton).BindEvent((pointerEventData) => {
            Debug.Log("로그인 버튼 클릭");
        });
    }

}
