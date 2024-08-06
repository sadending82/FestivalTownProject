using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId;
    private int maxPlayerNum = 6;
    private int nowPlayerNum;
    private bool createMyCharacter;

    private void Awake()
    {
        nowPlayerNum = 0;
        createMyCharacter = false;
    }

    private void Start()
    {
        //----- Client Test -----
        //SetMyId(0);
        //AddPlayers(2);
    }   
    public void SetMyId(int myId)
    {
        this.myId = myId;
        nowPlayerNum = myId + 1;
    }

    public int GetMyId()
    {
        return this.myId;
    }

    public void AddPlayer()
    {
        // �� ĳ���͸� ���� �������� üũ, ���� �����ϱ� ���� ������ �÷��̾�� �� ĳ���͸� ����
        if (createMyCharacter == false)
        {
            for (int i = 0; i < nowPlayerNum; ++i)
            {
                transform.GetChild(i).gameObject.SetActive(true);
                if (myId == i)
                {
                    createMyCharacter = true;
                    transform.GetChild(i).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                    transform.GetChild(i).gameObject.GetComponent<PlayerController>().SetMyId(myId);
                }
            }
        }
        // ���� ������ �Ŀ� ���ο� �÷��̾ ���������� ����
        else
        {
            if (nowPlayerNum + 1 > maxPlayerNum)
            {
                Debug.Log("Error!!! : You Can't Add Player, Maximum Number of Players");
            }
            else
            {
                transform.GetChild(nowPlayerNum).gameObject.SetActive(true);
                nowPlayerNum++;
            }
        }
    }

    // Ŭ���̾�Ʈ �׽�Ʈ��
    private void AddPlayers(int maxPlayerNum)
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            transform.GetChild(i).gameObject.SetActive(true);
            if (myId == i)
            {
                createMyCharacter = true;
                transform.GetChild(i).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                transform.GetChild(i).gameObject.GetComponent<PlayerController>().SetMyId(myId);
            }
        }
    }
}
