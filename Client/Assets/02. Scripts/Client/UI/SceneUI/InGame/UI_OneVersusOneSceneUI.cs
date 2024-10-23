using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using TMPro;
using UnityEngine;

public class UI_OneVersusOneSceneUI : UI_Scene
{
    enum GameObjects
    {
        CharacterImageTeam1,
        CharacterImageTeam2,
        PointPanelTeam1,
        PointPanelTeam2,
        TimerBackGround,
        TimerText,
        StaminaBar,
        HPBar,
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

    public int GetPoint(int team)
    {
        switch (team)
        {
            case 0:
                return Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().GetPoint();
            case 1:
                return Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().GetPoint();
            default:
                return -1;
        }
    }

    public void SetTimer(float remain_time)
    {

        int min = (int)(remain_time / 60.0f);
        int sec = (int)(remain_time % 60.0f);

        Get<GameObject>((int)GameObjects.TimerText).GetComponent<TMP_Text>().text = string.Format("{0:D}:{1:D2}", min, sec);

    }

    private void Update()
    {
        if (Managers.Game.isInGame)
        {
            // UI는 어디까지나 관찰하는 입장으로 데이터가 바뀌었는지 주기적으로 확인하는 방식.
            // 실제 데이터는 GameManager가 관리한다.

            for (int i = 0; i < 2; i++)
            {
                if (Managers.Game.TeamLife.TryGetValue(i, out int life)) // 값이 없으면 null이라 문제가 되니까 묶지 말고 따로 해야함
                {
                    if (life != GetPoint(i) && GetPoint(i) != -1)
                    {
                        SetPoint(i, life);
                    }
                }
            }


            SetTimer(Managers.Game.GameRemainTime);



            CharacterStatus character = null;

            try
            {
                character = Managers.Player.GetMyPlayer().GetComponent<CharacterStatus>();
            }
            catch
            {
                return;
            }

            if (character == null) return;

            if (Get<GameObject>((int)GameObjects.HPBar) == null) return;
            
            Get<GameObject>((int)GameObjects.HPBar).GetComponent<UI_HPBar>().SetHP(character.GetHp(), character.GetMaxHp());
            Get<GameObject>((int)GameObjects.StaminaBar).GetComponent<UI_StaminaBar>().SetStamina(character.GetStamina(), character.GetMaxStamina());
            
        }
    }
}
