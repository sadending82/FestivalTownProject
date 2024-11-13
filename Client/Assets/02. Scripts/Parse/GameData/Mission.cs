using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Mission : ScriptableObject
{
	public List<MissionTypeEntity> Type_Mission; 
	public List<MissionListEntity> List_Mission;
}
