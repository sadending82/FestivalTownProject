using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_ResultPanel : UI_Base
{
    enum GameObjects
    {
        Label,
        PlayerResult1,
        PlayerResult2,
        PlayerResult3,
        PlayerResult4,
        PlayerResult5,
        PlayerResult6,
    }

    bool isInitialized = false;

    int currentActivePanel = 0;

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        for(int i = 1; i < (int)GameObjects.PlayerResult6 + 1; ++i)
        {
            Get<GameObject>(i).GetComponent<UI_PlayerResult>().Init();
            Get<GameObject>(i).gameObject.SetActive(false);
        }

        isInitialized = true;

    }

    public void SetPlayerResult(string nickname, int playerId, int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        GetPlayerResultObj(currentActivePanel).SetActive(true);
        GetPlayerResultObj(currentActivePanel).GetComponent<UI_PlayerResult>().SetPlayerResult(playerId, nickname, kill, death, bombInsert, gold, isMvp);
        currentActivePanel++;
    }

    GameObject GetPlayerResultObj(int index)
    {
        if (index < 0) return null;

        return Get<GameObject>(index + 1);
    }
}
