using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;

public class UI_PlayerResult : UI_Base
{
    enum GameObjects
    {
        MVPEmphasis,
        NickNameText,
        KillText,
        DeathText,
        BombInsertText,
        GoldText,
        MVPImage,
        TeamFlag,
        PlayerImage,
    }

    bool isInitialzed = false;

    void Start()
    {
       if(!isInitialzed) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.TeamFlag).GetComponent<UI_TeamFlag>().Init();

        isInitialzed = true;
    }

    public void SetPlayerResult(int id, string nickname, int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        Get<GameObject>((int)GameObjects.NickNameText).GetComponent<TMP_Text>().text = nickname;
        if (Get<GameObject>((int)GameObjects.KillText) == null) Debug.Log("text null");
        if (Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>() == null) Debug.Log("tmp null");
        Debug.Log(Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>().text);
        Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>().text = kill.ToString();

        Get<GameObject>((int)GameObjects.DeathText).GetComponent<TMP_Text>().text = death.ToString();
        Get<GameObject>((int)GameObjects.BombInsertText).GetComponent<TMP_Text>().text = bombInsert.ToString();
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = gold.ToString();

        if (!isMvp)
        {
            Get<GameObject>((int)GameObjects.MVPEmphasis).SetActive(false);
            Get<GameObject>((int)GameObjects.MVPImage).SetActive(false);
        }

        int playerTeam = Managers.Game.PlayerTeamData[id];
        

        Get<GameObject>((int)GameObjects.TeamFlag).GetComponent<UI_TeamFlag>().SetFlag(playerTeam, playerTeam == Managers.Game.GetWinningTeam());
    }
}
