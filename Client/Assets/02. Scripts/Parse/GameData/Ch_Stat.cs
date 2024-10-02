using ExcelDataStructure;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset (AssetPath = "Resources/Prefabs/Data")]
public class Ch_Stat : ScriptableObject
{
	public List<CharacterStatEntity> Stat;
	public List<CharacterAttackEntity> Attack;
	public List<CharacterMoveEntity> Move;
	public List<CharacterActionEntity> Action;

}
