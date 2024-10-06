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
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GachaButton).BindEvent((PointerEventData) =>
        {
            Debug.Log("킹치만 아직 뽑기 기능은 없음 ㅋ");
        });

        Get<GameObject>((int)GameObjects.BackButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(GetComponent<UI_Present>());
        });
    }
}
