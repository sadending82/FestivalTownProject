using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_CreateAccountPanel : UI_Base
{

    const int EmailMaxLength = 12;
    const int EmailMinLength = 4;

    enum GameObjects
    {
        Title,
        Labels,
        EmailAddressInputField,
        AuthorizationButton,
        NickNameInputField,
        PWInputField,
        PWDoubleCheckInputField,
        EmailAddressCheckCircle,
        NickNameCheckCircle,
        PWCheckCircle,
        PWDoubleCheckCircle,
        
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public int AuthorizeEmail()
    {
        return 0;
    }

    public int EmailCheck()
    {
        // Email 적합성 판정
        string emailText = Get<GameObject>((int)GameObjects.EmailAddressInputField).GetComponent<TMP_InputField>().text;    

        if (emailText == "")
        {
            // 아직 볼 필요 없음.
            return -1;
        }
        else
        {

            if (emailText.Length > EmailMaxLength || emailText.Length < EmailMinLength)
            {
                return 0;
            }       

            foreach (var ch in emailText)
            {
                if (!Util.IsEnglish(ch) && !Util.IsNumeric(ch))
                {
                    return 0;
                }
            }

            return 1;
        }
    }

    public int NickNameCheck()
    {
        string NickNameText = Get<GameObject>((int)GameObjects.NickNameInputField).GetComponent<TMP_InputField>().text;

        if (NickNameText == "")
        {
            return -1;
        }
        else
        {
            bool isKorean = false;

            foreach (var ch in NickNameText)
            {
                if (Util.IsKorean(ch)) isKorean = true;
                break;
            }

            if (isKorean)
            {
                if (NickNameText.Length < 2 || NickNameText.Length > 8) {
                    return 0;
                }
            }
            else
            {
                if (NickNameText.Length < 4 || NickNameText.Length > 18)
                {
                    return 0;
                }
            }

            foreach (var ch in NickNameText)
            {
                if (!Util.IsKorean(ch) && !Util.IsEnglish(ch) && !Util.IsNumeric(ch))
                {
                    return 0;
                }
            }

            return 1;
        }
    }

    void Update()
    {
        Debug.Log("Panel Update");
        var emailCircle = Get<GameObject>((int)GameObjects.EmailAddressCheckCircle).GetComponent<UI_CompatibilityCircle>();

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

        

    }
}
