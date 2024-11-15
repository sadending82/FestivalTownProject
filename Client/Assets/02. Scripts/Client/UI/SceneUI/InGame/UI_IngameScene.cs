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
    /// ����Ʈ �г� ��ġ�� Ŭ���̾�Ʈ ���� �ٸ��ϱ�, Ŭ���̾�Ʈ ���� ����Ʈ �г��� ��ġ�� ������ �ֱ� ���� �Լ�
    /// </summary>
    public void PointPanelPositionSet()
    {
        switch(Managers.Game.mapCode)
        {
            case NetworkProtocol.MapCode.Map_FITH_1vs1 :
                SetPanelPositionToTeamGame();
                break;
            case NetworkProtocol.MapCode.Map_FITH_1vs1vs1:
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
                break;
            case 1: // 1 0
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(1);
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
                // 0 1 2
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(0);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(1);

                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            case 1:
                // 1 0 2
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(1);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            case 2:
                // 2 0 1
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam3);
                Get<GameObject>((int)GameObjects.PointPanelTeam3).GetComponent<UI_PointPanel>().SetTeam(0);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam1);
                Get<GameObject>((int)GameObjects.PointPanelTeam1).GetComponent<UI_PointPanel>().SetTeam(1);
                teamPanelIndex.Add((int)GameObjects.PointPanelTeam2);
                Get<GameObject>((int)GameObjects.PointPanelTeam2).GetComponent<UI_PointPanel>().SetTeam(2);
                break;
            default:
                break;
        }
    }

    public void SetPoint(int team, int point)
    {
        // ���� 0�� ���� �Դٸ�, 0�� ���� �ش��ϴ� ���� ���� ����Ʈ �г��� ��ġ�� ã�Ƽ� �ش� �г��� ������ ����.
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
            // UI�� �������� �����ϴ� �������� �����Ͱ� �ٲ������ �ֱ������� Ȯ���ϴ� ���.
            // ���� �����ʹ� GameManager�� �����Ѵ�.

            for (int i = 0; i < 2; i++)
            {
                if (Managers.Game.TeamLife.TryGetValue(i, out int life)) // ���� ������ null�̶� ������ �Ǵϱ� ���� ���� ���� �ؾ���
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
