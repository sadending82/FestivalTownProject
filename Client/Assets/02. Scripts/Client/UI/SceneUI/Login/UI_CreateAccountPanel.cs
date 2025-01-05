using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_CreateAccountPanel : UI_Base
{

    const int IDMaxLength = 20;
    const int IDMinLength = 4;

    enum GameObjects
    {
        Title,
        Labels,
        IdInputField,
        AuthorizationButton,
        NickNameInputField,
        PWInputField,
        PWDoubleCheckInputField,
        IdCheckCircle,
        NickNameCheckCircle,
        PWCheckCircle,
        PWDoubleCheckCircle,
        ToolTipImage,
    }

    bool isInitialized = false;

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.AuthorizationButton).BindEvent((PointerEventData) =>
        {
            if (EmailCheck() == 1)
            {
                Managers.Network.GetPacketManager().SendCheckID_DuplicationPacket(GetEmailText());
            }
            else
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("입력한 아이디가 유효하지 않습니다.");
                ui.BindPopupCloseEvent();
            }
        });

        Get<GameObject>((int)GameObjects.ToolTipImage).BindEvent((PointerEventData) =>
        {
            Get<GameObject>((int)GameObjects.ToolTipImage).transform.GetChild(0).gameObject.SetActive(true);
        }, Define.UIEvent.PointerEnter);

        Get<GameObject>((int)GameObjects.ToolTipImage).BindEvent((PointerEventData) =>
        {
            Get<GameObject>((int)GameObjects.ToolTipImage).transform.GetChild(0).gameObject.SetActive(false);
        }, Define.UIEvent.PointerExit);

        isInitialized = true;
    }

    public int AuthorizeId()
    {
        return 0;
    }

    public int EmailCheck()
    {

        // Email 적합성 판정

        // 왜인지는 모르겠지만 영어+숫자 4~12글자 라고 함
        string emailText = Get<GameObject>((int)GameObjects.IdInputField).GetComponent<TMP_InputField>().text;    

        if (emailText == "")
        {
            // 아직 볼 필요 없음.
            return -1;
        }
        else
        {
            char firstSpecial = '_';
            char secondSpecial = '-';

            if (emailText.Length < 5 || emailText.Length > 20) return 0;

            bool isAllSpecial = true;

            foreach(var ch in emailText)
            {
                if(ch != firstSpecial && ch != secondSpecial)
                {
                    isAllSpecial = false;
                    break;
                }
            }

            if (isAllSpecial) return 0;

            foreach (var ch in emailText)
            {
                if (!Util.IsEnglish(ch) && !Util.IsNumeric(ch) && ch != firstSpecial && ch != secondSpecial)
                {
                    return 0;
                }
            }

            return 1;
        }
    }

    public int NickNameCheck()
    {
        // 닉네임 적합성
        string NickNameText = Get<GameObject>((int)GameObjects.NickNameInputField).GetComponent<TMP_InputField>().text;

        if (NickNameText == "")
        {
            return -1;
        }
        else
        {

            char firstSpecial = '_';
            char secondSpecial = '-';

            if (NickNameText.Length < 2 || NickNameText.Length > 12)
            {
                return 0;
            }

            bool isAllSpecial = true;

            foreach (var ch in NickNameText)
            {
                if (ch != firstSpecial && ch != secondSpecial)
                {
                    isAllSpecial = false;
                    break;
                }
            }

            if (isAllSpecial) return 0;

            foreach (var ch in NickNameText)
            {
                if (!Util.IsKorean(ch) && !Util.IsEnglish(ch) && !Util.IsNumeric(ch) && ch != firstSpecial && ch != secondSpecial)
                {
                    return 0;
                }
            }

            return 1;
        }
    }

    public int PasswordCheck()
    {
        string PasswordText = Get<GameObject>((int)GameObjects.PWInputField).GetComponent<TMP_InputField>().text;

        if (PasswordText == "")
        {
            return -1;
        }
        else
        {
            if (PasswordText.Length > 20 || PasswordText.Length < 8)
            {
                return 0;
            }

            foreach (var ch in PasswordText)
            {
                if (Util.IsKorean(ch))
                {
                    return 0;
                }
            }

            return 1;
        }
    }

    public int PasswordDoubleCheck()
    {
        string PasswordText = Get<GameObject>((int)GameObjects.PWInputField).GetComponent<TMP_InputField>().text;
        string PasswordTextForCheck = Get<GameObject>((int)GameObjects.PWDoubleCheckInputField).GetComponent<TMP_InputField>().text;


        if (PasswordTextForCheck == "")
        {
            return -1;
        }
        else
        {
            if (PasswordText != PasswordTextForCheck) return 0;

            return 1;
        }
    }

    public bool IsCompatible()
    {
        var emailCircle = Get<GameObject>((int)GameObjects.IdCheckCircle).GetComponent<UI_CompatibilityCircle>();
        var NickNameCircle = Get<GameObject>((int)GameObjects.NickNameCheckCircle).GetComponent<UI_CompatibilityCircle>();
        var PasswordCircle = Get<GameObject>((int)GameObjects.PWCheckCircle).GetComponent<UI_CompatibilityCircle>();
        var PasswordDoubleCheckCircle = Get<GameObject>((int)GameObjects.PWDoubleCheckCircle).GetComponent<UI_CompatibilityCircle>();

        if (emailCircle.isCompatible && NickNameCircle.isCompatible &&
            PasswordCircle.isCompatible && PasswordDoubleCheckCircle.isCompatible) {
            return true;
        }

        return false;
    }

    public string GetEmailText()
    {
        return Get<GameObject>((int)GameObjects.IdInputField).GetComponent<TMP_InputField>().text;
    }

    public string GetNickName()
    {
        return Get<GameObject>((int)GameObjects.NickNameInputField).GetComponent<TMP_InputField>().text;
    }

    public string GetPassword()
    {
        return Get<GameObject>((int)GameObjects.PWInputField).GetComponent<TMP_InputField>().text;
    }

    void Update()
    {
        var emailCircle = Get<GameObject>((int)GameObjects.IdCheckCircle).GetComponent<UI_CompatibilityCircle>();

        switch (EmailCheck())
        {
            case -1:
                emailCircle.SetColor(Define.CompatibilityColor.Brown);
                break;
            case 0:
                emailCircle.SetColor(Define.CompatibilityColor.Red);
                break;
            case 1:
                emailCircle.SetColor(Define.CompatibilityColor.Green);
                break;
        }

        // 닉네임 적합성 판정
        var NickNameCircle = Get<GameObject>((int)GameObjects.NickNameCheckCircle).GetComponent<UI_CompatibilityCircle>();

        switch (NickNameCheck())
        {
            case -1:
                NickNameCircle.SetColor(Define.CompatibilityColor.Brown);
                break;
            case 0:
                NickNameCircle.SetColor(Define.CompatibilityColor.Red);
                break;
            case 1:
                NickNameCircle.SetColor(Define.CompatibilityColor.Green);
                break;
        }

        var PasswordCircle = Get<GameObject>((int)GameObjects.PWCheckCircle).GetComponent<UI_CompatibilityCircle>();

        switch(PasswordCheck())
        {
            case -1:
                PasswordCircle.SetColor(Define.CompatibilityColor.Brown);
                break;
            case 0:
                PasswordCircle.SetColor(Define.CompatibilityColor.Red);
                break;
            case 1:
                PasswordCircle.SetColor(Define.CompatibilityColor.Green);
                break;
        }

        var PasswordDoubleCheckCircle = Get<GameObject>((int)GameObjects.PWDoubleCheckCircle).GetComponent<UI_CompatibilityCircle>();

        switch (PasswordDoubleCheck())
        {
            case -1:
                PasswordDoubleCheckCircle.SetColor(Define.CompatibilityColor.Brown);
                break;
            case 0:
                PasswordDoubleCheckCircle.SetColor(Define.CompatibilityColor.Red);
                break;
            case 1:
                PasswordDoubleCheckCircle.SetColor(Define.CompatibilityColor.Green);
                break;
        }
    }
}
