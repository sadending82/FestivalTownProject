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
        PointerDown,
        PointerUp,
        PointerEnter,
        PointerExit,
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
        ModeTip,
        WeaponStat,
        EventMain,
        EventList,
        MissionType,
        MissionList,
        MapTheme,
        MapData,
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
        Emo = 24,
    }

    public enum SelectedEventToggle
    {
        Announcement = 0,
        Event = 1,
    }

    public enum ItemGrade
    {
        Normal = 1,
        Rare = 2,
        Unique = 3,
    }

    public struct PlayerData
    {
        public int id;
        public int team;
        public Vector3 pos;
        public string nickname;
    }

    public struct PlayerResult
    {
        public int Id;
        public string Name;
        public int kill;
        public int death;
        public int bombInsert;
        public int gold;
        public int point;
        public bool isMvp;
    }

    public struct ItemData
    {
        public int ItemUid;
        public int ItemCode;
        public int Count;
        public int Type;
    }

    public struct AttendanceEventData
    {
        public int EventCode;
        public int DayCount;
        public bool isRewarded;
    }
}
