using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public class DataManager
{
    /// <summary>
    /// 스크립터블 오브젝트 등으로 저장된 데이터를 쉽게 가져오기 위해
    /// 런타임시 항상 데이터를 항상 들고 있도록 설정하는 매니저.
    /// </summary>

    public Dictionary<int, GameDataEntity> GameDataDict = new Dictionary<int, GameDataEntity>();

    public void Init()
    {
        LoadGameData();
    }

    void LoadGameData()
    {
        Ch_Stat data = Managers.Resource.Load<Ch_Stat>($"Data/Ch_Stat");
        if (data == null) return;

        foreach(CharacterStatEntity cs in data.Stat)
        {
            GameDataDict.Add(cs.Index, cs);
        }

        foreach(CharacterAttackEntity ca in data.Attack)
        {
            GameDataDict.Add(ca.Index, ca);
        }

        foreach(CharacterMoveEntity cm in data.Move)
        {
            GameDataDict.Add(cm.Index, cm);
        }

        foreach(CharacterActionEntity cac in data.Action)
        {
            GameDataDict.Add(cac.Index, cac);
        }
    }

}
