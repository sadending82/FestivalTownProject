using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Present : UI_PopUp
{

    enum GameObjects
    {
        Panel,
        Text,
        GachaButton,
        BackButton
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GachaButton).BindEvent((PointerEventData) =>
        {
            Debug.Log("ŷġ�� ���� �̱� ����� ���� ��");
        });

        Get<GameObject>((int)GameObjects.BackButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.CloseAllPopUpUI();
            Managers.UI.ShowPopUpUI<UI_HomeStart>();
        });
    }
}