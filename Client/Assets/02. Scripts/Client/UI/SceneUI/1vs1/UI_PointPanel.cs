using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_PointPanel : UI_Base
{
    enum GameObjects
    {
        Point1,
        Point2,
        Point3,
    }

    int RemainPoint = 3;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetPoint(int value)
    {
        RemainPoint = value;

        AllPointOff();

        if(value > 0)
        {
            Get<GameObject>((int)GameObjects.Point1).GetComponent<UI_Point>().SetState(true);
        }

        if(value > 1)
        {
            Get<GameObject>((int)GameObjects.Point2).GetComponent<UI_Point>().SetState(true);
        }

        if(value > 2)
        {
            Get<GameObject>((int)GameObjects.Point3).GetComponent<UI_Point>().SetState(true);
        }
    }

    public void AllPointOff()
    {
        Get<GameObject>((int)GameObjects.Point1).GetComponent<UI_Point>().SetState(false);
        Get<GameObject>((int)GameObjects.Point2).GetComponent<UI_Point>().SetState(false);
        Get<GameObject>((int)GameObjects.Point3).GetComponent<UI_Point>().SetState(false);
    }
}
