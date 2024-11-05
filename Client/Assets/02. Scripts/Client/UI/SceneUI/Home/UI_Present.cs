using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Present : UI_PopUp
{

    enum GameObjects
    {
        Panel,
        TitleText,
        CharacterImage,
        BackButton,
        GachaSelectPanel,
    }

    void Start()
    {    
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.BackButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(GetComponent<UI_Present>());
        });
        
    }
}
