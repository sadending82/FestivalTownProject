using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId = -1;
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

        if(Managers.Game.inGameID != -1)
        {
            myId = Managers.Game.inGameID;
            isHost = Managers.Game.isHost;
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
        if (players == null) Init();

        if(Managers.Scene.CurrentScene.GetComponent<GameScene>() == null)
        {
            StartCoroutine(WaitAndAddPlayer(id));
            return;
        }

        if (nowPlayerNum + 1 > maxPlayerNum)
        {
            Debug.Log("Error!!! : You Can't Add Player, Maximum Number of Players");
        }
        else
        {
            Debug.Log($"Add Player {id}");
            players.transform.GetChild(id).gameObject.SetActive(true);
            nowPlayerNum++;          
        }
    }

    IEnumerator WaitAndAddPlayer(int id)
    {
        yield return new WaitUntil(() => Managers.Scene.CurrentScene.GetComponent<GameScene>() != null);
        AddPlayer(id);
    }

    public void SetMyPlayerEnable()
    {
        if (myId != -1)
        {
            GameObject tPlayer = FindPlayerById(myId);
            tPlayer.SetActive(true);
            tPlayer.GetComponent<CharacterStatus>().SetAmIPlayer(true);
            tPlayer.GetComponent<PlayerController>().SetMyId(myId);
        }

    }
    public GameObject FindPlayerById(int targetId)
    {
        for (int i=0;i<maxPlayerNum;++i)
        {
            if (players.transform.GetChild(i).GetComponent<CharacterStatus>().GetId() == targetId)
            {
                return players.transform.GetChild(i).gameObject;
            }
        }

        Debug.Log("ERROR!!! : FindPlayerById(), Can't Find Target Player By Id : " + targetId);
        return null;
    }
}
