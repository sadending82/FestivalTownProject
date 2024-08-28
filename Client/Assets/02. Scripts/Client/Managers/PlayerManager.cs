using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId;
    private int maxPlayerNum;
    private int nowPlayerNum;
    private bool isHost;

    private void Awake()
    {
        nowPlayerNum = 0;
        maxPlayerNum = 6;
    }

    private void Start()
    {
    }   
    public void SetMyId(int myId)
    {
        this.myId = myId;
        Debug.Log("My ID is " + myId);
    }

    public int GetMyId()
    {
        return this.myId;
    }

    public void SetIsHost(bool flag)
    {
        this.isHost = flag;
        if (flag)
        {
            Debug.Log("I'm Game Host ");
        }
    }

    public bool GetIsHost()
    {
        return this.isHost;
    }

    public void AddPlayer(int id)
    {
        if (nowPlayerNum + 1 > maxPlayerNum)
        {
            Debug.Log("Error!!! : You Can't Add Player, Maximum Number of Players");
        }
        else
        {
            if (transform.GetChild(id).gameObject.activeSelf == false)
            {
                transform.GetChild(id).gameObject.SetActive(true);
                if (myId == id)
                {
                    transform.GetChild(id).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                    transform.GetChild(id).gameObject.GetComponent<PlayerController>().SetMyId(myId);
                }
                Debug.Log("Add Player ID : " + nowPlayerNum);
                nowPlayerNum++;
                Debug.Log("Now Players Number " + nowPlayerNum);
            }
        }
    }
}
