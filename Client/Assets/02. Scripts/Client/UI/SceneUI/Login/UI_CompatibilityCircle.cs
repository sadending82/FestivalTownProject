using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_CompatibilityCircle : UI_Base
{

    public bool isCompatible = false;

    enum GameObjects
    {
        BrownCircle,
        RedCircle,
        GreenCircle,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        SetColor(Define.CompatibilityColor.Brown);
    }

    public void SetColor(Define.CompatibilityColor color)
    {
        AllObjActiveFalse();

        switch (color)
        {
            case Define.CompatibilityColor.Brown:
                Get<GameObject>((int)GameObjects.BrownCircle).SetActive(true);
                isCompatible = false;
                break;
            case Define.CompatibilityColor.Red:
                Get<GameObject>((int)GameObjects.RedCircle).SetActive(true);
                isCompatible = false;
                break;
            case Define.CompatibilityColor.Green:
                Get<GameObject>((int)GameObjects.GreenCircle).SetActive(true);
                isCompatible = true;
                break;
        }
    }

    public void AllObjActiveFalse()
    {
        Get<GameObject>((int)GameObjects.BrownCircle).SetActive(false);
        Get<GameObject>((int)GameObjects.RedCircle).SetActive(false);
        Get<GameObject>((int)GameObjects.GreenCircle).SetActive(false);
    }
}
