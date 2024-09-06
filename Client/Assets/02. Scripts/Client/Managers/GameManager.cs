using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour 
{
    /// <summary>
    /// ���� ���ӿ� ���� �����͸� �����ϰ�, ������ �����ϴ� �༮.
    /// �����������, �ð��̶����
    /// ���� ���� �ΰ����� �ƴ϶�� Clear �ϰ��� ���� �� �ص� ��.
    /// </summary>

    public bool isInGame = false;
    public bool isTimerStart = false;
    public float GameRemainTime = 300.0f;
    public float GameEndTime = 300.0f;
    public Dictionary<int, int> TeamLife = new();
    public bool isHost = false;
    public int inGameID = -1;
    public int RoomID = -1;


    public void Init()
    {
        // �̰� �ӽ÷� ���� �Ŷ� ���߿� �ٲ���� �մϴ�
        isInGame = true;
        SetDefaultGameData(Define.GameMode.FireInTheHoleTeam1);
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
            return;
        }

        //GameEndTime = data.Play_Time;
    }
    
    public void SetLifeCount(int team, int life)
    {
        if (!isInGame) return; 

        TeamLife[team] = life;

    }

    private void Update()
    {
        if (!isInGame) return;

        if (GameRemainTime > 0 && isTimerStart)
        {
            GameRemainTime -= Time.deltaTime;
        }
    }
    public void SendToServerGoalTeamNumber(int bombId, int teamNumber)
    {
        PacketManager packetManager = Managers.Network.GetPacketManager();
        packetManager.SendBombInputPacket(bombId, teamNumber);
    }
}
