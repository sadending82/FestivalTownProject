using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour 
{
    /// <summary>
    /// ���� ���ӿ� ���� �����͸� �����ϴ� �༮
    /// �����������, �ð��̶����
    /// ���� ���� �ΰ����� �ƴ϶�� Clear �ϰ����� ���� �� �ص� ��.
    /// </summary>

    public bool isInGame = false;
    public float GameRemainTime = 300.0f;
    public float GameEndTime = 300.0f;
    public Dictionary<int, int> TeamLife = new();

    public void Init()
    {
        // �̰� �ӽ÷� ���� �Ŷ� ���߿� �ٲ���� �մϴ�
        isInGame = true;
    }

    public void SetDefaultGameData(Define.GameMode mode)
    {
        switch(mode)
        {
            case Define.GameMode.FireInTheHoleTeam1:
                for(int i = 0; i < 2; i++)
                {
                    TeamLife.Add(i, 3);
                }
                break;
            case Define.GameMode.FireInTheHoleTeam2:
                for (int i = 0; i < 2; i++)
                {
                    TeamLife.Add(i, 3);
                }
                break;
            case Define.GameMode.FireInTheHoleTeam3:
                for (int i = 0; i < 2; i++)
                {
                    TeamLife.Add(i, 3);
                }
                break;
            case Define.GameMode.FireInTheHoleIndivisual3:
                for (int i = 0; i < 3; i++)
                {
                    TeamLife.Add(i, 3);
                }
                break;
            case Define.GameMode.FireInTheHoleIndivisual5:
                for (int i = 0; i < 5; i++)
                {
                    TeamLife.Add(i, 3);
                }
                break;
        }

        Managers.Data.GameModeDataDict.TryGetValue(mode, out GameModeEntity data);
        if (data != null)
        {
            Debug.Log($"{mode}�� Null �̿���!!!!!!!.");
        }

        GameEndTime = data.Play_Time;
    }
    
    public void SetLifeCount(int team, int life)
    {
        if (!isInGame) return; 

        TeamLife[team] = life;

    }

    private void Update()
    {
        if (!isInGame) return;

        if (GameRemainTime > 0)
        {
            GameRemainTime -= Time.deltaTime;
        }
    }
}