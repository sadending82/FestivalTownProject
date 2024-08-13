using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public class DataManager
{
    /// <summary>
    /// ��ũ���ͺ� ������Ʈ ������ ����� �����͸� ���� �������� ����
    /// ��Ÿ�ӽ� �׻� �����͸� �׻� ��� �ֵ��� �����ϴ� �Ŵ���.
    /// </summary>

    public Dictionary<string, CharacterStatEntity> CharacterStatDataDict = new Dictionary<string, CharacterStatEntity>();
    public Dictionary<Define.GameMode, GameModeEntity> GameModeDataDict = new Dictionary<Define.GameMode, GameModeEntity>();

    public void Init()
    {
        LoadStatData();
        LoadGameModeData();
    }

    void LoadStatData() 
    { 
        CharacterStatData data = Managers.Resource.Load<CharacterStatData>($"Data/CharacterStatData");
        if (data == null) return;

        foreach(CharacterStatEntity entity in data.Sheet1)
        {
            CharacterStatDataDict.Add(entity.name, entity);
        }
    }

    void LoadGameModeData()
    {
        FITH_Mode data = Managers.Resource.Load<FITH_Mode>($"Data/FITH_Mode");
        if (data == null) return;

        foreach (TeamBattleOneEntity entity in data.Team_Battle_one)
        {
            GameModeDataDict.Add(Define.GameMode.FireInTheHoleTeam1, entity);
        }

        foreach (TeamBattleTwoEntity entity in data.Team_Battle_Two)
        {
            GameModeDataDict.Add(Define.GameMode.FireInTheHoleTeam2, entity);
        }

        foreach (TeamBattleThreeEntity entity in data.Team_Battle_Three)
        {
            GameModeDataDict.Add(Define.GameMode.FireInTheHoleTeam3, entity);
        }
    }

}
