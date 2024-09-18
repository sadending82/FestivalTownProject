using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI_HPBar : UI_Base
{
    enum GameObjects
    {
        HPImage,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetHP(float hp, float maxHp)
    {
        if(maxHp != 0)
        Get<GameObject>((int)GameObjects.HPImage).GetComponent<Image>().fillAmount = hp / maxHp;
    }
}
