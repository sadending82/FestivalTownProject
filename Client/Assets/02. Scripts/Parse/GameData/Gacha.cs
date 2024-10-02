using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset (AssetPath = "Resources/Prefabs/Data")]
public class Gacha : ScriptableObject
{
	public List<GachaGroupEntity> GachaGroup;
	public List<GachaGroupItemEntity> GachaGroup_Item;

}
