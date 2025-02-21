using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace ExcelDataStructure
{
    [System.Serializable]
    public abstract class GameDataEntity
    {
        public int Index;
        public string Name;
        public Define.ExcelDataClassType ClassType;
    }

    [System.Serializable]
    public class CharacterStatEntity : GameDataEntity
    {
        public float Ch_Hp;
        public int Ch_Stamina;
        public float Ch_Strength;
        public int Ch_Speed;
    }

    [System.Serializable]
    public class CharacterAttackEntity : GameDataEntity
    {
        public float Value;
        public float Attack_Speed;
        public float Attack_Range;
        public int Ch_StaminaConsume;
        public int Vanish_Stamina;
    }

    [System.Serializable]
    public class CharacterMoveEntity : GameDataEntity
    {
        public float Value;
        public int Ch_StaminaConsume;
        public int Ch_Stamina_recovery;
    }

    [System.Serializable]
    public class CharacterActionEntity : GameDataEntity
    {
        public float Action_Speed;
        public float Action_Range;
        public int Ch_StaminaConsume;
    }

    [System.Serializable]
    public class GachaGroupEntity : GameDataEntity
    {
        public int Gacha_Group;
        public int Pay_Item1_Index;
        public int Pay_Item1_Value;
        public int Pay_Item2_Index;
        public int Pay_Item2_Value;
        public int Open_Date;
        public int Close_Date;
        public bool Is_Visible;
    }

    [System.Serializable]
    public class GachaGroupItemEntity : GameDataEntity
    {
        public int Gacha_Group;
        public float Gacha_Weight;
        public int Reward_Item_Index;
        public float Reward_Item_Value;
        public bool Is_Promote;
    }

    [System.Serializable]
    public class ItemEntity : GameDataEntity
    {
        public string File_Name;
        public int Item_Type;
        public int Item_Grade;
        public int Open_Date;
        public int Close_Date;

    }

    [System.Serializable]
    public class ModeOutEntity
    {
        public int Mode_Index;
        public string Name;
        public Define.ExcelDataClassType ClassType;
        public string Name_Info;
        public int Player_Count;
        public int Team_Count;
        public int Team_Color;
        public int Play_Map;
        public int Open_Date;
        public int Close_Date;
    }

    [System.Serializable]
    public class FITHModeEntity
    {
        public int Map_Index;
        public int Mode_Index;
        public string Name;
        public Define.ExcelDataClassType ClassType; 
        public string Name_Info;
        public int Play_Time;
        public int Player_Spawn_Time;
        public int Life_Count;
        public int Bomb_Spawn_Count;
        public int Bomb_Spawn_Time;
        public int Bomb_Delay_Time;
        public int Bomb_Ready_Count1;
        public int Bomb_Ready_Time1;
        public int Bomb_Ready_Count2;
        public int Bomb_Ready_Time2;
        public int Weapon1_Spawn_Index;
        public int Weapon1_Spawn_Count;
        public int Weapon1_Spawn_Time;
        public int Weapon2_Spawn_Index;
        public int Weapon2_Spawn_Count;
        public int Weapon2_Spawn_Time;
        public int Block1_Spawn_Index;
        public int Block1_Spawn_Count;
        public int Block1_Spawn_Time;
        public int Block2_Spawn_Index;
        public int Block2_Spawn_Count;
        public int Block2_Spawn_Time;

    }

    [System.Serializable]
    public class ModePointEntity : GameDataEntity
    {
        public int Mode_Index;
        public int Battle_Result;
        public int Type;
        public int WorkType;
        public int Value;
    }

    [System.Serializable]
    public class ModeRewardEntity : GameDataEntity
    {
        public int Mode_Index;
        public int Battle_Result;
        public int Reward1_Index;
        public int Reward1_Value;
        public int Reward2_Index;
        public int Reward2_Value;
        public int Reward3_Index;
        public int Reward3_Value;
    }

    [System.Serializable]
    public class ModeBonusRewardEntity : GameDataEntity
    {
        public int Mode_Index;
        public int Battle_Result;
        public int Type;
        public int Point;
        public int Reward1_Index;
        public int Reward1_Value;
        public int Reward2_Index;
        public int Reward2_Value;
        public int Reward3_Index;
        public int Reward3_Value;

    }

    [System.Serializable]
    public class ModeTipEntity : GameDataEntity
    {
        public int Tip_Type;
        public string Tip_Description;
    }

    [System.Serializable]
    public class WeaponStatEntity : GameDataEntity
    {
        public int Weapon_Power;
        public float Weapon_Range;
        public int Weapon_Type;
        public int Weapon_StaminaConsume;
    }

    [System.Serializable]
    public class EventMainEntity
    {
        public int Event_Id;
        public string Name;
        public int Type;
        public int Open_Date;
        public int Close_Date;
        public Define.ExcelDataClassType ClassType;
    }

    [System.Serializable]
    public class EventListEntity
    {
        public int Index;
        public int EventList;
        public int Day;
        public int Reward_Item_Index;
        public int Reward_Item_Value;
        public int Open_Date;
        public int Close_Date;
        public bool is_received;
        public Define.ExcelDataClassType ClassType;
    }

    [System.Serializable]
    public class MissionTypeEntity : GameDataEntity
    {

    }

    [System.Serializable]
    public class MissionListEntity : GameDataEntity
    {
        public string Info;
        public int Type;
        public int Reset_Time;
        public int Open_Date;
        public int Close_Date;
        public int Overlap_Count;
        public int Reward1_Index;
        public int Reward1_value;
        public int Element1_Index;
        public int Element1_Count;
    }

    [System.Serializable]
    public class MapThemeEntity : GameDataEntity
    {
        public int Map_Theme;
        public string Mode_Name;
        public string Mode_Description;
        public string Map_Name;
        public string Map_Image;
        public string Minimap_Image;
    }

    [System.Serializable]
    public class MapDataEntity : GameDataEntity
    {
        public int Mode_index;
        public int Map_Size_X;
        public int Map_Size_Y;
        public int Statue_Team_Red;
        public int Statue_Team_Blue;
        public int Statue_Team_Green;
        public float Statue_Team_Red_LocationX;
        public float Statue_Team_Red_LocationY;
        public float Statue_Team_Red_LocationZ;
        public float Statue_Team_Blue_LocationX;
        public float Statue_Team_Blue_LocationY;
        public float Statue_Team_Blue_LocationZ;
        public float Statue_Team_Green_LocationX;
        public float Statue_Team_Green_LocationY;
        public float Statue_Team_Green_LocationZ;
        public int Statue_Team_Red_Direction;
        public int Statue_Team_Blue_Direction;
        public int Statue_Team_Green_Direction;
    }

    [System.Serializable]

    public class PassListEntity : GameDataEntity
    {
        public int Open_Date;
        public int Close_Date;
    }

    [System.Serializable]

    public class PassLevelEntity : GameDataEntity
    {
        public int Pass;
        public int Level;
        public int Exp_Required;
        public int Pass_Type;
        public int Reward_Item_Index;
        public int Reward_Item_Amount;
    }

    [System.Serializable]

    public class PassMissionEntity : GameDataEntity
    {
        public int Pass;
        public int Type;
        public int Mission_Category;
        public int Mission_Group;
        public int Mission_Step;
        public string Mission_Name;
        public string Mission_Description;
        public int Required_Count;
        public int Reward_Exp;
        public int Reward_Item;
        public int Reward_Item_Amount;

    }

    [System.Serializable]

    public class ShopCategoryEntity : GameDataEntity
    {
        public string Category_Name;
        public string Category_Description;
        public int Number_Goods;
        public int Number_Discountable;
        public int Reroll_Term;
        public int Open_Date;
        public int Close_Date;
    }

    [System.Serializable]

    public class ShopListEntity : GameDataEntity
    {
        public int Category_Index;
        public int Item;
        public int Item_Grade;
        public int Item_Amount;
        public int Currency1_Id;
        public int Currency1_Price;
        public int Currency2_Id;
        public int Currency2_Price;
        public int Purchase_Limit;
        public int Discount_Rate_Min;
        public int Discount_Rate_Max;
        public int Open_Date;
        public int Close_Date;
    }

}
