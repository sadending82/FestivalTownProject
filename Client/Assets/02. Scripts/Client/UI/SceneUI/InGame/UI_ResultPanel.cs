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

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        for(int i = 1; i < (int)GameObjects.PlayerResult6 + 1; ++i)
        {
            Get<GameObject>(i).gameObject.SetActive(false);
        }

        isInitialized = true;

    }

    public void SetPlayerResult(string nickname, int playerId, int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        GetPlayerResultObj(playerId).SetActive(true);
        GetPlayerResultObj(playerId).GetComponent<UI_PlayerResult>().SetPlayerResult(nickname, kill, death, bombInsert, gold, isMvp);
    }

    GameObject GetPlayerResultObj(int id)
    {
        if (id < 0) return null;
        //���߿� MaxPlayer�� �����ؼ� �װ͵� ����ó�� ������ҵ�

        // �ϴ��� PlayerResult ���� Label �ۿ� �����Ƿ� �ӽ� ����.
        return Get<GameObject>(id + 1);
    }
}
