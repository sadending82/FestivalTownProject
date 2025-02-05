using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Pass : ScriptableObject
{
	//public List<EntityType> Pass_Info; // Replace 'EntityType' to an actual type that is serializable.
	public List<PassListEntity> Pass_List; // Replace 'EntityType' to an actual type that is serializable.
	public List<PassMissionEntity> Pass_Mission; // Replace 'EntityType' to an actual type that is serializable.

}
