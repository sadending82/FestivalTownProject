using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Map : ScriptableObject
{
	public List<MapThemeEntity> Theme; // Replace 'EntityType' to an actual type that is serializable.
	public List<MapDataEntity> Data; // Replace 'EntityType' to an actual type that is serializable.
}
