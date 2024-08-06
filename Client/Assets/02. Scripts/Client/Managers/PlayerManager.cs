using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId;
    private int maxPlayerNum;
    private int nowPlayerNum;
    private bool createMyCharacter;

    private void Awake()
    {
        nowPlayerNum = 0;
        createMyCharacter = false;
        maxPlayerNum = 6;
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
        Debug.Log("My ID is " + myId);
    }

    public int GetMyId()
    {
        return this.myId;
    }

    public void AddPlayer()
    {
        Debug.Log("!!Add Player!!");
        // ���� ������ �Ŀ� ���ο� �÷��̾ ���������� ����
        if(createMyCharacter == true)
        {
            if (nowPlayerNum + 1 > maxPlayerNum)
            {
                Debug.Log("Error!!! : You Can't Add Player, Maximum Number of Players");
            }
            else
            {
                if (transform.GetChild(nowPlayerNum).gameObject.activeSelf == false)
                {
                    transform.GetChild(nowPlayerNum).gameObject.SetActive(true);
                    Debug.Log("Add Player ID : " + nowPlayerNum);
                }
                nowPlayerNum++;
            }
        }
        // �� ĳ���͸� ���� �������� üũ, ���� �����ϱ� ���� ������ �÷��̾�� �� ĳ���͸� ����
        else
        {
            createMyCharacter = true;
            for (int i = myId; i >= 0; --i)
            {
                if (transform.GetChild(i).gameObject.activeSelf == false)
                {
                    transform.GetChild(i).gameObject.SetActive(true);
                    Debug.Log("Add Player ID : " + i);
                    if (myId == i)
                    {
                        transform.GetChild(i).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                        transform.GetChild(i).gameObject.GetComponent<PlayerController>().SetMyId(myId);
                    }
                }
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
                transform.GetChild(i).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                transform.GetChild(i).gameObject.GetComponent<PlayerController>().SetMyId(myId);
            }
        }
    }
}
