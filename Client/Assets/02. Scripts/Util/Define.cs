using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Define
{

    public enum Sound
    {
        Bgm,
        Effect,
    }
    public enum Scene
    {
        None,
        Login,
        Home,
        Loading,
        Game,
    }
    public enum GameMode
    {
        None,
        FireInTheHoleTeam1,
        FireInTheHoleTeam2,
        FireInTheHoleTeam3,
        FireInTheHoleIndivisual3,
        FireInTheHoleIndivisual5,
    }

    public enum StatueState
    { 
        Destroyed,
        AttackedTwoTime,
        AttackedOneTime,
        Fine,

    }
    public enum CharacterType
    {
        Neru,

    }

    public enum UIEvent
    {
        Click,
        Drag,
    }

    public enum MouseEvent
    {
        Press,
        Click,
    }

    public enum ExcelDataClassType
    {
        CharacterStat,
        CharacterAttack,
        CharacterMove,
        CharacterAction,
        GachaGroup,
        GachaGroupItem,
        Item,
        ModeOut,
        FITHMode,
        ModePoint,
        ModeReward,
        ModeBonusReward,
        WeaponStat,
        EventMain,
        EventList,
        MissionType,
        MissionList,
    }

    public enum CompatibilityColor
    {
        Brown,
        Red,
        Green,
    }

    public enum ItemType
    {
        Resource = 1,
        Box = 5,
        Skin = 10,
        Accessory_Head = 21,
        Accessory_Face = 22,
        Accessory_Back = 23,
    }

    public struct PlayerResult
    {
        public string Name;
        public int kill;
        public int death;
        public int bombInsert;
        public int gold;
        public bool isMvp;
    }

    public struct ItemData
    {
        public int ItemUid;
        public int ItemCode;
        public int Count;
        public int Type;
    }
}
