using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset]
public class FITH_Mode : ScriptableObject
{
	public List<TeamBattleOneEntity> Team_Battle_one; 
	public List<TeamBattleTwoEntity> Team_Battle_Two; 
	public List<TeamBattleThreeEntity> Team_Battle_Three; 
}
