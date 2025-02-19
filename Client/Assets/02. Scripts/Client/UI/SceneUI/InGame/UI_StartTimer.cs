using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_StartTimer : UI_PopUp
{
    enum GameObjects
    {
        Three,
        Two,
        One,
        Go,
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

    public void TimerSet(int num)
    {
        AllObjActiveFalse();

        switch (num)
        {
            case 0:
                Get<GameObject>((int)GameObjects.Go).SetActive(true);
                break;
            case 1:
                Get<GameObject>((int)GameObjects.One).SetActive(true);
                break;
            case 2:
                Get<GameObject>((int)GameObjects.Two).SetActive(true);
                break;
            case 3:
                Get<GameObject>((int)GameObjects.Three).SetActive(true);
                break;
        }
    }

    public void AllObjActiveFalse()
    {
        Get<GameObject>((int)GameObjects.Three).SetActive(false);
        Get<GameObject>((int)GameObjects.Two).SetActive(false);
        Get<GameObject>((int)GameObjects.One).SetActive(false);
        Get<GameObject>((int)GameObjects.Go).SetActive(false);
    }
}
