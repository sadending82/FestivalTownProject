using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ExcelDataStructure;

[ExcelAsset(LogOnImport = true)]
public class CharacterStatData : ScriptableObject
{
	public List<CharacterStatEntity> Sheet1; // Replace 'EntityType' to an actual type that is serializable.

}
