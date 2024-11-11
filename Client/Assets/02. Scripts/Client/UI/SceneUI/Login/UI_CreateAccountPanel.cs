using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
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

        // Email ���ռ� ����

        // �������� �𸣰����� ����+���� 4~12���� ��� ��
        string emailText = Get<GameObject>((int)GameObjects.EmailAddressInputField).GetComponent<TMP_InputField>().text;    

        if (emailText == "")
        {
            // ���� �� �ʿ� ����.
            return -1;
        }
        else
        {
            char firstSpecial = '@';
            char secondSpecial = '.';

            bool isFirstFound = false;
            bool isSecondFound = false;

            foreach (var ch in emailText)
            {
                if (!Util.IsEnglish(ch) && !Util.IsNumeric(ch))
                {
                    if (!isFirstFound)
                    {
                        if(ch == firstSpecial)
                        {
                            isFirstFound = true;
                            continue;
                        }
                    }
                    else if (!isSecondFound)
                    {
                        if(ch == secondSpecial)
                        {
                            isSecondFound = true;
                            continue;
                        }
                    }
                    return 0;
                }
            }

            return 1;
        }
    }

    public int NickNameCheck()
    {
        // �г��� ���ռ�

        // �ѱ��� ��� 4~8, ����,������ ��� 8~18�̶�� ��.
        // �ٵ� �ѱ� ���� ���� �� ���̸� ����?
        // �ϴ��� �ѱ��� �����ϴ� ���� �ѱ۷� ����ϱ�� �Ͽ���.
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

    public int PasswordCheck()
    {
        string PasswordText = Get<GameObject>((int)GameObjects.PWInputField).GetComponent<TMP_InputField>().text;

        if (PasswordText == "")
        {
            return -1;
        }
        else
        {
            if (PasswordText.Length > 15 || PasswordText.Length < 8)
            {
                return 0;
            }

            bool isEnglishContain = false;
            bool isNumericContain = false;

            foreach (var ch in PasswordText)
            {
                if (Util.IsEnglish(ch)) isEnglishContain = true;
                if (Util.IsNumeric(ch)) isNumericContain = true;

                if (isEnglishContain && isNumericContain) break;
            }

            if (!isEnglishContain || !isNumericContain) return 0;

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
        var emailCircle = Get<GameObject>((int)GameObjects.EmailAddressCheckCircle).GetComponent<UI_CompatibilityCircle>();
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
        return Get<GameObject>((int)GameObjects.EmailAddressInputField).GetComponent<TMP_InputField>().text;
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

        // �г��� ���ռ� ����
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
