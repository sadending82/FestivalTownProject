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
        public int Pay_Item_Index;
        public int Pay_Item_Value;
        public int Open_Date;
        public int Close_Date;
        public float Exp;
    }

    [System.Serializable]
    public class GachaGroupItemEntity : GameDataEntity
    {
        public int Gacha_Group;
        public float Gacha_Weight;
        public int Reward_Item_Index;
        public float Reward_Item_Value;
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
    public class ModeOutEntity : GameDataEntity
    {
        public string Name_Info;
        public int Player_Count;
        public int Team_Count;
        public int Team_Color;
        public int Play_Map;
        public int Open_Date;
        public int Close_Date;
    }

    [System.Serializable]
    public class FITHModeEntity : GameDataEntity
    {
        public string Name_Info;
        public int Play_Time;
        public int Player_Spawn_Time;
        public int Life_Count;
        public int Bomb_Spawn_Count;
        public int Bomb_Spawn_Time;
        public int Bomb_Delay_Time;
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
    public class WeaponStatEntity : GameDataEntity
    {
        public int Weapon_Power;
        public float Weapon_Range;
        public int Weapon_Type;
        public int Weapon_StaminaConsume;
    }
}
