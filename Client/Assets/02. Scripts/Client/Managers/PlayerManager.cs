using Google.FlatBuffers;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;

public class PlayerManager : MonoBehaviour
{
    private int myId = -1;
    private int maxPlayerNum = 6;
    private int nowPlayerNum = 0;
    private bool isHost;

    [SerializeField] private GameObject players;
    
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
            CharacterStatus playerStatus = player.GetComponent<CharacterStatus>();
            player.name = playerPrefeb.name + " " + i;
            playerStatus.SetId(i);
            playerStatus.SetLayer(i);
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

    public void AddPlayer(int id, Vector3 position, int teamNumber, string name)
    {
        if (Managers.Scene.CurrentScene.GetComponent<GameScene>() == null)
        {
            StartCoroutine(WaitAndAddPlayer(id, position, teamNumber, name));
            return;
        }

        if (players == null) Init();

        if (nowPlayerNum + 1 > maxPlayerNum)
        {
            Debug.Log("Error!!! : You Can't Add Player, Maximum Number of Players");
        }
        else
        {
            var playerObject = players.transform.GetChild(id).gameObject;
            playerObject.SetActive(true);
            playerObject.GetComponent<CharacterStatus>().SetTeamNumber(teamNumber);
            playerObject.GetComponent<CharacterStatus>().SetNickName(name);
            playerObject.GetComponent<PlayerController>().SetMyId(id);

            List<int> itemList = Managers.Game.GetCharacterCustomizingById(id);
            foreach(int itemCode in itemList)
            {
                playerObject.GetComponent<CharacterStatus>().ChangeCustomizing(itemCode);
            }

            playerObject.GetComponent<PlayerController>().FirstSpawn(position.x, position.z);
            nowPlayerNum++;
        }
    }

    IEnumerator WaitAndAddPlayer(int id, Vector3 position, int teamNumber, string name)
    {
        yield return new WaitUntil(() => Managers.Scene.CurrentScene.GetComponent<GameScene>() != null);
        AddPlayer(id, position, teamNumber, name);
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
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            if (players.transform.GetChild(i).GetComponent<CharacterStatus>().GetId() == targetId)
            {
                return players.transform.GetChild(i).gameObject;
            }
        }

        Debug.Log("ERROR!!! : FindPlayerById(), Can't Find Target Player By Id : " + targetId);
        return null;
    }
    public GameObject GetMyPlayer()
    {
        return FindPlayerById(myId);
    }

    public void GameStart()
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            GameObject tPlayer = players.transform.GetChild(i).gameObject;
            if (tPlayer.activeSelf == true)
            {
                tPlayer.GetComponent<CharacterStatus>().GameStart();
            }
        }
    }
    public void GameEnd()
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            GameObject tPlayer = players.transform.GetChild(i).gameObject;
            if (tPlayer.activeSelf == true)
            {
                tPlayer.GetComponent<CharacterStatus>().GameEnd();
            }
        }
    }
    public void SetCameraInPlayerUIControllers(Camera targetCamera)
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            if (players.transform.GetChild(i).gameObject.activeSelf == true)
            {
                players.transform.GetChild(i).GetComponent<CharacterStatus>().SetUINowCamera(targetCamera);
            }
        }
    }
    public void Clear()
    {
        myId = -1;
        maxPlayerNum = 6;
        nowPlayerNum = 0;
        isHost = false;

        foreach (Transform player in players.transform)
        {
            Managers.Resource.Destroy(player.gameObject);
        }
    }
}
