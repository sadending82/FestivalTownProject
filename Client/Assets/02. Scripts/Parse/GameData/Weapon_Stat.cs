using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Weapon_Stat : ScriptableObject
{
	public List<WeaponStatEntity> Stat;
}
