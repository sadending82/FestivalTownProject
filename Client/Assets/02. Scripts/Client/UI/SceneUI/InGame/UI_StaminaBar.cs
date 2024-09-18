using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI_StaminaBar : UI_Base
{
    enum GameObjects
    {
        StaminaImage,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }


    public void SetStamina(float stamina, float maxStamina)
    {
        if(maxStamina != 0)
        Get<GameObject>((int)GameObjects.StaminaImage).GetComponent<Image>().fillAmount = stamina / maxStamina;
    }
}
