using System.Collections;
using System.Collections.Generic;
using UnityEditor.Experimental.GraphView;
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



    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        for(int i = 1; i < (int)GameObjects.PlayerResult6 + 1; ++i)
        {
            Get<GameObject>(i).gameObject.SetActive(false);
        }

    }

    public void SetPlayerResult(int playerId, int kill, int death, int bombInsert, int gold)
    {
        GetPlayerResultObj(playerId).SetActive(true);
        GetPlayerResultObj(playerId).GetComponent<UI_PlayerResult>().SetPlayerResult(kill, death, bombInsert, gold);
    }

    GameObject GetPlayerResultObj(int id)
    {
        if (id < 0) return null;
        //���߿� MaxPlayer�� �����ؼ� �װ͵� ����ó�� ������ҵ�

        // �ϴ��� PlayerResult ���� Label �ۿ� �����Ƿ� �ӽ� ����.
        return Get<GameObject>(id + 1);
    }
}