using NetworkProtocol;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using TMPro;
using UnityEngine;

public class UI_IngameScene: UI_Scene
{
    enum GameObjects
    {
        PointPanelTeam1,
        PointPanelTeam2,
        PointPanelTeam3,
        TimerBackGround,
        TimerText,
        HPBar,
        StaminaBar,
    }

    bool isInitialized = false;
    int _currentClientTeam = -1;
    List<int> teamPanelIndex = new();

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().Init();
        Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().Init();
        Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().Init();

        isInitialized = true;
    }

    public void SetCurrentClientTeam(int team)
    {
        _currentClientTeam = team;
    }

    /// <summary>
    /// 포인트 패널 위치가 클라이언트 마다 다르니까, 클라이언트 별로 포인트 패널의 위치를 지정해 주기 위한 함수
    /// </summary>
    public void PointPanelPositionSet()
    {
        switch(Managers.Game.gameMode)
        {
            
            case eGameMode.FITH_Team_Battle_4:
                SetPanelPositionToTeamGame();
                break;
            case eGameMode.FITH_Team_Battle_6:
                SetPanelPositionToTeamGame();
                break;
            case eGameMode.FITH_Indiv_Battle_2:
                SetPanelPositionToTeamGame();
                break;
            case eGameMode.FITH_Indiv_Battle_3:
                SetPanelPositionToIndivisualGame();
                break;
            default:
                break;
        }
    }

    public void SetPanelPositionToTeamGame()
    {
        switch(_currentClientTeam)
        {
            case 0: // 0 1
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(1);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).SetActive(false);
                break;
            case 1: // 1 0
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(1);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).SetActive(false);
                break;
            default:
                break;
        }
    }

    public void SetPanelPositionToIndivisualGame()
    {
        switch (_currentClientTeam)
        {
            case 0:
                // 자기 자신의 팀이 빨강.
                // panel1 -> 0, panel2 -> 1, panel3 -> 2
                // 패널 넣는 순서는 1, 2, 3 이어야 함.
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(0);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(1);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            case 1:
                // 자기 자신의 팀이 파랑
                // panel1 -> 1, panel2 -> 0, panel3 -> 2
                // 패널 인덱스는, 2, 1, 3의 순서여야 함.
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(0);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(1);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            case 2:
                // 자기 자신의 팀이 초록
                // panel1 -> 2, panel2 -> 0, panel3 -> 1
                // 패널 인덱스는 2, 3, 1의 순서여야 함.
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(1);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            default:
                break;
        }
    }

    public void SetPoint(int team, int point)
    {
        // 만약 0번 팀이 왔다면,
        // 0번 팀에 해당하는 빨간 팀의 포인트 패널의 위치를 찾아서 해당 패널의 점수를 세팅.
        Get<GameObject>(teamPanelIndex[team]).GetComponent<UI_PointPanel>().SetPoint(point);
    }

    public int GetPoint(int team)
    {
        return Get<GameObject>(teamPanelIndex[team]).GetComponent<UI_PointPanel>().GetPoint();
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

            for (int i = 0; i < Managers.Game.TeamLife.Count; i++)
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
