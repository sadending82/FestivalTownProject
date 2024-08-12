using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public class DataManager
{
    public Dictionary<string, CharacterStatEntity> CharacterStatDict = new Dictionary<string, CharacterStatEntity>();

    public void Init()
    {
        LoadStatData();
    }

    void LoadStatData() 
    { 
        CharacterStatData data = Managers.Resource.Load<CharacterStatData>($"Data/CharacterStatData");
        if (data == null) return;

        foreach(CharacterStatEntity entity in data.Sheet1)
        {
            CharacterStatDict.Add(entity.name, entity);
        }
    }

}
