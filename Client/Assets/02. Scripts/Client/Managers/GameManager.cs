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
    public int mapIndex = -1;
    public int mapTheme = -1;
    public eGameMode gameMode;
    public bool isAnounnced15s = false;
    public List<Define.PlayerResult> PlayerResultData = new();
    /// <summary>
    /// pair -> ID, Team
    /// </summary>
    public Dictionary<int, int> PlayerTeamData = new();
    private int winningTeam = -1;
    public int currentTeamID = -1;

    public string TipDescription = "";
    public List<int> TipIndices = new();
    public string ModeTitle = "";
    public string ModeDescription = "";

    private List<List<int>> playersCustomizingData = new List<List<int>>();
    private Dictionary<int, Define.PlayerData> playerInitialDataDict = new();

    public List<int> GetCharacterCustomizingById(int id)
    {
        return playersCustomizingData[id];
    }
    public void SetCharacterCustomizingById(int id, int itemCode)
    {
        playersCustomizingData[id].Add(itemCode);
    }
    public void ClearCharacterCustomizing()
    {
        playersCustomizingData.Clear();
        for (int i = 0; i < 6; ++i)
        {
            playersCustomizingData.Add(new List<int>());
        }
    }

    public Dictionary<int, Define.PlayerData> GetPlayerDataDict()
    {
        return playerInitialDataDict;
    }

    public void SetPlayerData(int id, Define.PlayerData data)
    {
        playerInitialDataDict[id] = data;
    }

    public void AddPlayerData(int id, Define.PlayerData data)
    {
        playerInitialDataDict.Add(id, data);
    }

    public void ClearPlayerData()
    {
        playerInitialDataDict.Clear();
    }

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
        Debug.Log($"{team} 팀, 라이프를 {life}로");
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
        PlayerTeamData.Clear();
        winningTeam = -1;
        ClearCharacterCustomizing();
        ClearPlayerData();
    }

    public void LoadGameMap()
    {
        Debug.Log("Load Game Map !!!");
        Managers.Map.LoadGameMap(mapIndex, mapTheme);
        switch (gameMode)
        {
            case eGameMode.FITH_Indiv_Battle_2:
                {
                    SetDefaultGameData(Define.GameMode.FireInTheHoleTeam1);
                }
                break;
            case eGameMode.FITH_Indiv_Battle_3:
                {
                    SetDefaultGameData(Define.GameMode.FireInTheHoleIndivisual3);
                }
                break;
            case eGameMode.FITH_Team_Battle_4:
                {
                    SetDefaultGameData(Define.GameMode.FireInTheHoleTeam1);
                }
                break;
            case eGameMode.FITH_Team_Battle_6:
                {
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
        isGameEnd = true;
        isTimerStart = false;
        Managers.Player.GameEnd();
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
