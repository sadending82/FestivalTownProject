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
        CharacterImageTeam1,
        CharacterImageTeam2,
        CharacterImageTeam3,
    }

    bool isInitailized = false;
    int RemainPoint = 3;
    int CurrentTeam = 0;

    void Start()
    {
        if(!isInitailized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        RemainPoint = 3;
        isInitailized = true;
    }

    public void SetTeam(int team)
    {
        CurrentTeam = team;
        AllImageOff();
        Get<GameObject>((int)GameObjects.CharacterImageTeam1 + team).SetActive(true);
    }

    public void AllImageOff()
    {
        Get<GameObject>((int)GameObjects.CharacterImageTeam1).SetActive(false);
        Get<GameObject>((int)GameObjects.CharacterImageTeam2).SetActive(false);
        Get<GameObject>((int)GameObjects.CharacterImageTeam3).SetActive(false);
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

    public int GetPoint()
    {
        return RemainPoint;
    }
}
