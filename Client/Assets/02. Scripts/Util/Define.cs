using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Define
{
    public enum Scene
    {
        None,
        Login,
        WaitingRoom,
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
}