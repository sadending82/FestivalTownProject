using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.EventSystems.EventTrigger;


/// <summary>
/// 스크립터블 오브젝트 등으로 저장된 데이터를 쉽게 가져오기 위해
/// 런타임시 항상 데이터를 항상 들고 있도록 설정하는 매니저.
/// 나중에 public으로 설정되어 있는 녀석들은 전부 getter, setter 설정을 해줘야 한다. (보안 문제)
/// </summary>
public class DataManager
{

    public enum ResourceIndexType
    {
        None,
        Gold,
        Diamond,
        Mileage,
        TicketBase,
        TicketRare,
    }

    const int GoldIndex = 100001;
    const int DiamondIndex = 100002;
    const int MileageIndex = 100003;
    const int TicketBaseIndex = 100004;
    const int TicketRareIndex = 100005;

    bool isInventoryDataRecved = false;

    /// <summary>
    /// 데이터들이 들어가 있는 딕셔너리
    /// </summary>
    public Dictionary<int, GameDataEntity> GameDataDict = new(); // 나중에 겹치는 경우가 생길 수 있으므로 따로 분리해 두라고 함.
    public Dictionary<int, ItemEntity> ItemDict = new();
    public Dictionary<int, GachaGroupEntity> GachaGroupDict = new();
    public Dictionary<int, GachaGroupItemEntity> GachaGroupItemDict = new();
    public Dictionary<int, ModeOutEntity> ModeOutDataDict = new();
    public Dictionary<int, FITHModeEntity> FITHModeDataDict = new();
    public Dictionary<int, ModePointEntity> ModePointDataDict = new();
    public Dictionary<int, ModeRewardEntity> ModeRewardDataDict = new();
    public Dictionary<int, ModeBonusRewardEntity> ModeBonusRewardDataDict = new();
    public Dictionary<int, EventMainEntity> EventMainDataDict = new();
    public Dictionary<int, EventListEntity> EventListDataDict = new();
    public Dictionary<int, MissionTypeEntity> MissionTypeDataDict = new();
    public Dictionary<int, MissionListEntity> MissionListDataDict = new();

    /// <summary>
    /// 플레이어 관련 데이터
    /// 페어 타입 -> 아이템 인덱스, 보유량 
    /// </summary>
    public Dictionary<int, int> PlayerDataDict = new();
    public List<Define.ItemData> InventoryDataList = new();

    /// <summary>
    /// 페어 타입 -> 아이템 타입, 아이템 데이터
    /// </summary>
    public Dictionary<int, Define.ItemData> PlayerCustomizingData = new();

    /// <summary>
    /// 인벤토리에서 모델 띄울 때 로컬로 사용할 딕셔너리.
    /// 선택하면 뭐가 바뀌는 지 알아야 하니까.
    /// 아이템 타입, 아이템 인덱스
    /// </summary>
    public Dictionary<int, Define.ItemData> ClientLocalCustomizingDataDict = new();

    /// <summary>
    /// 출석 이벤트 데이터 저장용
    /// </summary>
    public Dictionary<int, Define.AttendanceEventData> AttendanceEventDataDict = new();
    bool hasAttendanceToday = false;
    bool hasAttendanceDataRecved = false;
    bool hasAttendanceUIPopUp = false;

    string NickName = "DefaultNick";


    public void Init()
    {
        LoadGameData();
        LoadModeData();

        Define.ItemData item = new Define.ItemData();
        foreach(Define.ItemType type in Enum.GetValues(typeof(Define.ItemType)))
        {
            item.Type = (int)type;
            item.ItemUid = -1;
            item.ItemCode = -1;

            PlayerCustomizingData.Add((int)type, item);
        }  
    }

    public GameDataEntity GetData(int dataIndex)
    {
        GameDataDict.TryGetValue(dataIndex, out GameDataEntity data);
        if (data == null)
        {
            return null;
        }

        return data;
    }

    public ItemEntity GetItemData(int dataIndex)
    {
        ItemDict.TryGetValue(dataIndex, out ItemEntity data);
        if (data == null)
        {
            return null;
        }

        return data;
    }

    public GachaGroupEntity GetGachaGroupData(int dataIndex)
    {
        GachaGroupDict.TryGetValue(dataIndex, out GachaGroupEntity data);
        if (data == null)
        {
            return null;
        }

        return data;
    }

    public GachaGroupItemEntity GetGachaGroupItemData(int dataIndex)
    {
        GachaGroupItemDict.TryGetValue(dataIndex, out GachaGroupItemEntity data);
        if (data == null)
        {
            return null;
        }

        return data;
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
                GachaGroupDict.TryAdd(entity.Index, entity);
                GachaGroupDict[entity.Index].ClassType = Define.ExcelDataClassType.GachaGroup;
            }

            foreach(GachaGroupItemEntity entity in gachaData.GachaGroup_Item)
            {
                GachaGroupItemDict.TryAdd(entity.Index, entity);
                GachaGroupItemDict[entity.Index].ClassType = Define.ExcelDataClassType.GachaGroupItem;
            }
        }

        Item itemData = Managers.Resource.Load<Item>($"Data/item");
        if (itemData != null)
        {
            foreach(ItemEntity entity in itemData.Item_List)
            {
                ItemDict.TryAdd(entity.Index, entity);
                if(entity.Index >= 100000 && entity.Index <= 100999)
                {
                    PlayerDataDict.TryAdd(entity.Index, -1);
                }
                ItemDict[entity.Index].ClassType = Define.ExcelDataClassType.Item;
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

        Event eventData = Managers.Resource.Load<Event>($"Data/Event");
        if (eventData != null)
        {
            foreach(EventMainEntity entity in eventData.Event_Main)
            {
                EventMainDataDict.TryAdd(entity.Event_Id, entity);
                EventMainDataDict[entity.Event_Id].ClassType = Define.ExcelDataClassType.EventMain;
            }

            foreach(EventListEntity entity in eventData.Event_List)
            {
                EventListDataDict.TryAdd(entity.Index, entity);
                EventListDataDict[entity.Index].ClassType = Define.ExcelDataClassType.EventList;
            }
        }

        Mission missionData = Managers.Resource.Load<Mission>($"Data/Mission");
        if (missionData != null)
        {
            foreach(MissionTypeEntity entity in missionData.Type_Mission)
            {
                MissionTypeDataDict.TryAdd(entity.Index, entity);
                MissionTypeDataDict[entity.Index].ClassType = Define.ExcelDataClassType.MissionType;
            }

            foreach(MissionListEntity entity in  missionData.List_Mission)
            {
                MissionListDataDict.TryAdd(entity.Index, entity);
                MissionListDataDict[entity.Index].ClassType = Define.ExcelDataClassType.MissionList;
            }
        }
    }

    public int GetDiamond()
    {
        return PlayerDataDict[DiamondIndex];
    }
    public void SetDiamond(int value)
    {
        PlayerDataDict[DiamondIndex] = value;
    }

    public int GetGold()
    {
        return PlayerDataDict[GoldIndex];
    }

    public void SetGold(int value)
    {
        PlayerDataDict[GoldIndex] = value;
    }

    public void SetNickName(string value)
    {
        NickName = value;
    }

    public string GetNickName()
    {
        return NickName;
    }

    public void SetTicketBase(int value)
    {
        PlayerDataDict[TicketBaseIndex] = value;
    }

    public int GetTicketBase()
    {
        return PlayerDataDict[TicketBaseIndex];
    }

    public void SetTicketRare(int value)
    {
        PlayerDataDict[TicketRareIndex] = value;
    }

    public int GetTicketRare()
    {
        return PlayerDataDict[TicketRareIndex];
    }

    public void SetMileage(int value)
    {
        PlayerDataDict[MileageIndex] = value;
    }

    public int GetMileage()
    {
        return PlayerDataDict[MileageIndex];
    }

    public int GetPlayerData(int index)
    {
        return PlayerDataDict[index];
    }

    public ResourceIndexType GetResourceIndexType(int index)
    {
        if (index == GoldIndex) return ResourceIndexType.Gold;
        if (index == DiamondIndex) return ResourceIndexType.Diamond;
        if (index == MileageIndex) return ResourceIndexType.Mileage;
        if (index == TicketBaseIndex) return ResourceIndexType.TicketBase;
        if (index == TicketRareIndex) return ResourceIndexType.TicketRare;

        return ResourceIndexType.None;
    }

    public bool IsInventoryDataRecved()
    {
        return isInventoryDataRecved;
    }

    public void SetInventoryDataRecved(bool value)
    {
        isInventoryDataRecved = value;
    }

    public bool HasAttendanceToday()
    {
        return hasAttendanceToday;
    }

    public void SetAttendanceToday(bool value)
    {
        hasAttendanceToday = value;
    }

    public bool HasAttendanceDataRecved()
    {
        return hasAttendanceDataRecved;
    }

    public void SetAttendanceDataRecved(bool value)
    {
        hasAttendanceDataRecved = value;
    }

    public bool HasAttendanceUIPopUp()
    {
        return hasAttendanceUIPopUp;
    }

    public void SetAttendanceUIPopUp(bool value)
    {
        hasAttendanceUIPopUp = value;
    }
}
