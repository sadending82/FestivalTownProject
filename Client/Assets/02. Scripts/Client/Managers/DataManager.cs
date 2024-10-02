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
