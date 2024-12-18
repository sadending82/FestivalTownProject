using SuperBlur;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_LoginPanel: UI_Base
{
    enum GameObjects
    {
        IDInputField,
        IDImage,
        PWInputField,
        PWImage,
        CreateAccountButton,
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
            Login();
        });
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Tab))
        {
            if (EventSystem.current.currentSelectedGameObject == null)
            {
                Get<GameObject>((int)GameObjects.IDInputField).GetComponent<Selectable>().Select();
            }
            else
            {
                Selectable next = EventSystem.current.currentSelectedGameObject.GetComponent<Selectable>().FindSelectableOnDown();

                if (next != null)
                {
                    next.Select();
                }
            }
        }
    }

    public void Login()
    {
        string ID = Get<GameObject>((int)GameObjects.IDInputField).GetComponent<TMP_InputField>().text;
        string PW = Get<GameObject>((int)GameObjects.PWInputField).GetComponent<TMP_InputField>().text;
        Debug.Log($"ID : {ID}, PW : {PW}");
        Managers.Network.GetPacketManager().SendLoginRequestPacket(ID, PW);
    }

}
