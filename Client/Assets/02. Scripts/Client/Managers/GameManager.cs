using ExcelDataStructure;
using NetworkProtocol;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 현재 게임에 대한 데이터를 저장하고, 게임을 관리하는 녀석.
/// 라이프라던지, 시간이라던지
/// 현재 씬이 인게임이 아니라면 Clear 하고나서 관리 안 해도 됨.
/// </summary>
public class GameManager : MonoBehaviour 
{

    public bool isInGame = false;
    public bool isGameEnd = false;
    public bool isTimerStart = false;
    public float GameRemainTime = 300.0f;
    public float GameEndTime = 300.0f;
    public Dictionary<int, int> TeamLife = new();
    public bool isHost = false;
    public int inGameID = -1;
    public int RoomID = -1;
    public MapCode mapCode;
    public bool isAnounnced15s = false;
    public Dictionary<int, Define.PlayerResult> PlayerResultData = new();
    private int winningTeam = -1;
    public int currentTeamID = -1;
    

    public void Init()
    {
        Clear();
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
            if(GameRemainTime < 15.0f && isAnounnced15s == false)
            {
                isAnounnced15s = true;
                Managers.Sound.Play("Sfx_15sec");
            }
        }
    }


    public void SendToServerGoalTeamNumber(int bombId, int teamNumber, int playerId)
    {
        if (isGameEnd) return;
        PacketManager packetManager = Managers.Network.GetPacketManager();
        packetManager.SendBombInputPacket(bombId, teamNumber, playerId);
    }

    public void Clear()
    {
        isTimerStart = false;
        isInGame = false;
        //GameRemainTime = 300.0f;
        TeamLife = new();
        isHost = false;
        inGameID = -1;
        RoomID = -1;
        currentTeamID = -1;
        isAnounnced15s = false;
        isGameEnd = false;
        PlayerResultData.Clear();
        winningTeam = -1;
    }

    public void LoadGameMap()
    {
        GameObject maps = GameObject.Find("Maps");
        switch(mapCode)
        {
            case MapCode.Map_FITH_1vs1vs1:
                {
                    maps.transform.GetChild(0).gameObject.SetActive(true);
                    SetDefaultGameData(Define.GameMode.FireInTheHoleIndivisual3);
                }
                break;
            case MapCode.Map_FITH_1vs1:
                {
                    maps.transform.GetChild(1).gameObject.SetActive(true);
                    SetDefaultGameData(Define.GameMode.FireInTheHoleTeam1);
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! LoadGameMap(): Wrong MapCode!!!");
                }
                break;
        }
    }

    public void GameStart()
    {
        isTimerStart = true;
        Managers.Network.GetPacketManager().GameStart();
        Managers.SpectatorCamera.AddCameras();
        Managers.Player.GameStart();
    }

    public void GameEnd()
    {
        isTimerStart = false;
        Managers.Player.GameEnd();
        Managers.Network.GetPacketManager().GameEnd();
        Managers.ObjectPool.Clear();
        Managers.CubeObject.Clear();
        Managers.BombObject.Clear();
        Managers.WeaponObject.Clear();

        isGameEnd = true;
        isInGame = false;
    }

    public void SetWinningTeam(int winningTeam)
    {
        this.winningTeam = winningTeam;
    }
    public int GetWinningTeam()
    {
        return winningTeam;
    }
    public bool GetisGameEnd()
    {
        return isGameEnd;
    }
}
