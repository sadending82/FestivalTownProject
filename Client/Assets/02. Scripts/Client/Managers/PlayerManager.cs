using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId;
    private int maxPlayerNum = 6;
    private int nowPlayerNum = 0;
    private bool isHost;

    [SerializeField]
    private GameObject players;

    public void Init()
    {
        players = GameObject.Find("@Players");
        if (players == null)
        {
            players = new GameObject { name = "@Players" };
        }
        GameObject playerPrefeb = Managers.Resource.Load<GameObject>($"Prefabs/Chibi_Cat");
        if (playerPrefeb == null)
        {
            Debug.Log("Error!! PlayerManager Init() Thers is no Prefeb !!!");
        }
        for (int i = 0; i < maxPlayerNum; i++)
        {
            GameObject player = Object.Instantiate(playerPrefeb, players.transform);
            player.name = playerPrefeb.name + " " + i;
            player.GetComponent<CharacterStatus>().SetId(i);
            player.GetComponent<CharacterStatus>().SetLayer(i);
            player.SetActive(false);
        }
    }
    public GameObject GetPlayers()
    {
        return players;
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
            if (players.transform.GetChild(id).gameObject.activeSelf == false)
            {
                players.transform.GetChild(id).gameObject.SetActive(true);
                if (myId == id)
                {
                    players.transform.GetChild(id).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
                    players.transform.GetChild(id).gameObject.GetComponent<PlayerController>().SetMyId(myId);
                }
                nowPlayerNum++;
            }
        }
    }
}
