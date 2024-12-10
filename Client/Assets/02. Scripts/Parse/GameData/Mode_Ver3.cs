using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset(AssetPath = "Resources/Data")]
public class Mode_Ver3 : ScriptableObject
{
	public List<ModeOutEntity> Mode_Out; // Replace 'EntityType' to an actual type that is serializable.
	public List<FITHModeEntity> FITH_Mode; // Replace 'EntityType' to an actual type that is serializable.
	public List<ModePointEntity> Mode_Point; // Replace 'EntityType' to an actual type that is serializable.
	public List<ModeRewardEntity> Mode_Reward; // Replace 'EntityType' to an actual type that is serializable.
	public List<ModeBonusRewardEntity> Mode_BonusReward; // Replace 'EntityType' to an actual type that is serializable.
	public List<ModeTipEntity> Tip;

}
