using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Shop : ScriptableObject
{
	public List<ShopCategoryEntity> Shop_Category; // Replace 'EntityType' to an actual type that is serializable.
	public List<ShopListEntity> Shop_List; // Replace 'EntityType' to an actual type that is serializable.
}
