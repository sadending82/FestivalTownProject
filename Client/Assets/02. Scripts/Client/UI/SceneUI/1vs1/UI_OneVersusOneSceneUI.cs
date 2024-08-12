using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_OneVersusOneSceneUI : UI_Scene
{
    enum GameObjects
    {
        CharacterImageTeam1,
        CharacterImageTeam2,
        PointPanelTeam1,
        PointPanelTeam2,
        TimerText
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetPoint(int team, int point)
    {
        switch(team)
        {
            case 0:
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetPoint(point);
                break;
            case 1:
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetPoint(point);
                break;
        }
    }
}
