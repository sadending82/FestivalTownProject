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
    }

    public enum CompatibilityColor
    {
        Brown,
        Red,
        Green,
    }

    public struct PlayerResult
    {
        public int kill;
        public int death;
        public int bombInsert;
        public int gold;
        public bool isMvp;
    }
}
