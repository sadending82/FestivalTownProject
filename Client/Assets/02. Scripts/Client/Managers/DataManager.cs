using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.EventSystems.EventTrigger;


/// <summary>
/// 스크립터블 오브젝트 등으로 저장된 데이터를 쉽게 가져오기 위해
/// 런타임시 항상 데이터를 항상 들고 있도록 설정하는 매니저.
/// </summary>
public class DataManager
{

    /// <summary>
    /// 데이터들이 들어가 있는 딕셔너리
    /// </summary>
    public Dictionary<int, GameDataEntity> GameDataDict = new();
    public Dictionary<int, ModeOutEntity> ModeOutDataDict = new();
    public Dictionary<int, FITHModeEntity> FITHModeDataDict = new();
    public Dictionary<int, ModePointEntity> ModePointDataDict = new();
    public Dictionary<int, ModeRewardEntity> ModeRewardDataDict = new();
    public Dictionary<int, ModeBonusRewardEntity> ModeBonusRewardDataDict = new();

    public void Init()
    {
        LoadGameData();
        LoadModeData();
    }

    public GameDataEntity GetData(int dataIndex)
    {
        return GameDataDict[dataIndex];
    }

    void LoadModeData()
    {
        Mode_Ver3 modeData = Managers.Resource.Load<Mode_Ver3>($"Data/Mode_Ver3");
        if (modeData != null)
        {
            foreach (ModeOutEntity entity in modeData.Mode_Out)
            {
                ModeOutDataDict.TryAdd(entity.Index, entity);
                ModeOutDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeOut;
            }

            foreach (FITHModeEntity entity in modeData.FITH_Mode)
            {
                FITHModeDataDict.TryAdd(entity.Index, entity);
                FITHModeDataDict[entity.Index].ClassType = Define.ExcelDataClassType.FITHMode;
            }

            foreach (ModePointEntity entity in modeData.Mode_Point)
            {
                ModePointDataDict.TryAdd(entity.Index, entity);
                ModePointDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModePoint;
            }

            foreach (ModeRewardEntity entity in modeData.Mode_Reward)
            {
                ModeRewardDataDict.TryAdd(entity.Index, entity);
                ModeRewardDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeReward;
            }

            foreach (ModeBonusRewardEntity entity in modeData.Mode_BonusReward)
            {
                ModeBonusRewardDataDict.TryAdd(entity.Index, entity);
                ModeBonusRewardDataDict[entity.Index].ClassType = Define.ExcelDataClassType.ModeBonusReward;
            }
        }
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
