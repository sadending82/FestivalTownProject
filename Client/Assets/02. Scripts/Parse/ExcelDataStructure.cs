using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace ExcelDataStructure
{

    [System.Serializable]
    public class GameModeEntity
    {
        public float Play_Time;
        public float Player_Spawn_Time;
        public float Team_Life_Count;
        public float Bomb_Spawn_Count;
        public float Bomb_Spawn_Time;
        public float Bomb_Spawn_Location_MinX;
        public float Bomb_Spawn_Location_MaxX;
        public float Bomb_Spawn_Location_MinY;
        public float Bomb_Spawn_Location_MaxY;
        public float Weapon_Spawn_Time;
        public float Weapon_Spawn_Count;
        public float Block_Spawn_Count;
        public float Block_Spawn_Time;
        public float Block_Spawn_Location_MinX;
        public float Block_Spawn_Location_MaxX;
        public float Block_Spawn_Location_MinY;
        public float Block_Spawn_Location_MaxY;
    }

    [System.Serializable]
    public class TeamBattleOneEntity : GameModeEntity
    {

    }

    [System.Serializable]
    public class TeamBattleTwoEntity : GameModeEntity
    {

    }

    [System.Serializable]
    public class TeamBattleThreeEntity : GameModeEntity
    { 
    }

    [System.Serializable]
    public class CharacterAttackVariableEntity
    {

    }

    [System.Serializable]
    public class CharacterStatEntity
    {
        public int index;
        public string name;
        public float Ch_Hp;
        public float Ch_Stamina;
        public float Ch_Strength;
        public float Ch_Speed;
        public float Ch_Attack;
    }
}
