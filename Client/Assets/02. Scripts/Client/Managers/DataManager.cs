using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using UnityEngine;
using static UnityEngine.EventSystems.EventTrigger;


/// <summary>
/// ��ũ���ͺ� ������Ʈ ������ ����� �����͸� ���� �������� ����
/// ��Ÿ�ӽ� �׻� �����͸� �׻� ��� �ֵ��� �����ϴ� �Ŵ���.
/// </summary>
public class DataManager
{

    /// <summary>
    /// �����͵��� �� �ִ� ��ųʸ�
    /// </summary>
    public Dictionary<int, GameDataEntity> GameDataDict = new();

    public void Init()
    {
        LoadGameData();
    }

    public GameDataEntity GetData(int dataIndex)
    {
        return GameDataDict[dataIndex];
    }

    void LoadGameData()
    {
        Ch_Stat StatData = Managers.Resource.Load<Ch_Stat>($"Data/Ch_Stat");
        if (StatData != null)
        {

            foreach (CharacterStatEntity entity in StatData.Stat)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.CharacterStat;
            }

            foreach (CharacterAttackEntity entity in StatData.Attack)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.CharacterAttack;
            }

            foreach (CharacterMoveEntity entity in StatData.Move)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.CharacterMove;
            }

            foreach (CharacterActionEntity entity in StatData.Action)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.CharacterAction;
            }
        }

        Gacha gachaData = Managers.Resource.Load<Gacha>($"Data/Gacha");
        if (gachaData != null)
        {
            foreach(GachaGroupEntity entity in gachaData.GachaGroup)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.GachaGroup;
            }

            foreach(GachaGroupItemEntity entity in gachaData.GachaGroup_Item)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.GachaGroupItem;
            }
        }

        Item itemData = Managers.Resource.Load<Item>($"Data/Gacha");
        if (itemData != null)
        {
            foreach(ItemEntity entity in itemData.Item_List)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.Item;
            }
        }

        Mode_Ver3 modeData = Managers.Resource.Load<Mode_Ver3>($"Data/Mode_Ver3");
        if (modeData != null)
        {
            foreach(ModeOutEntity entity in  modeData.Mode_Out)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeOut;
            }

            foreach (FITHModeEntity entity in modeData.FITH_Mode)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.FITHMode;
            }

            foreach (ModePointEntity entity in modeData.Mode_Point)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModePoint;
            }

            foreach (ModeRewardEntity entity in modeData.Mode_Reward)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeReward;
            }

            foreach (ModeBonusRewardEntity entity in modeData.Mode_BonusReward)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeBonusReward;
            }
        }

        Weapon_Stat weaponData = Managers.Resource.Load<Weapon_Stat>($"Data/Weapon_Stat");
        if (weaponData != null)
        {
            foreach(WeaponStatEntity entity in weaponData.Stat)
            {
                GameDataDict.TryAdd(entity.Index, entity);
                GameDataDict[entity.Index].ClassType = Define.ExcelDataClassType.WeaponStat;
            }
        }
    }

}
