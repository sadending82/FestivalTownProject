using ExcelDataStructure;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExcelAsset (AssetPath = "Resources/Data")]
public class Event : ScriptableObject
{
	public List<EventMainEntity> Event_Main;
	public List<EventListEntity> Event_List;
}
