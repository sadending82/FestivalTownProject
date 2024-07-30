using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace ExcelDataStructure
{
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
