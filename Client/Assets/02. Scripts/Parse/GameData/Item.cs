using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset (AssetPath = "Resources/Prefabs/Data")]
public class Item : ScriptableObject
{
	public List<ItemEntity> Item_List;
}
