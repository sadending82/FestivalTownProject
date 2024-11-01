using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_CreateAccount : UI_PopUp
{

    enum GameObjects
    {
        Blocker,
        CreateAccountPanel,
        CreateAccountButton,
        QuitButton,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.CreateAccountButton).BindEvent((PointerEventData) =>
        {
            Debug.Log("회원가입 버튼 클릭");

            var panel = Get<GameObject>((int)GameObjects.CreateAccountPanel).GetComponent<UI_CreateAccountPanel>();
            if (panel.IsCompatible())
            {
                Debug.Log($"ID : {panel.GetEmailText()}");
                Debug.Log($"PW : {panel.GetPassword()}");
                Debug.Log($"NAME : {panel.GetNickName()}");

                Managers.Network.GetPacketManager().SendSignUpRequestPacket(
                    panel.GetEmailText(), panel.GetPassword(), panel.GetNickName());
            }
            else
            {
                Debug.Log("회원가입 실패");
            }
        });

        Get<GameObject>((int)GameObjects.QuitButton).BindEvent((PointerEventData) =>
        {
            if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
            {
                Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
            }
            Managers.UI.ClosePopUpUI();
        });
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape)) {
            Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
            ClosePopUpUI();
        }
    }

}
