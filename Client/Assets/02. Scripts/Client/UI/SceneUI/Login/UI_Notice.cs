using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Notice : UI_PopUp 
{
    enum GameObjects
    {
        Blocker,
        NoticePanel,
        OkButton,
        NoticeText,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));
    }

    public void NoticeTextChange(string str)
    {
        Get<GameObject>((int)GameObjects.NoticeText).GetComponent<TMP_Text>().text = str;
    }

}
