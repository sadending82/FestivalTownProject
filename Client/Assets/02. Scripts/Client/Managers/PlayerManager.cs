using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : MonoBehaviour
{
    private int myId;
    private int maxPlayer = 2;

    public void SetMyId(int myId)
    {
        this.myId = myId;
    }

    public void AddPlayers()
    {
        for (int i = 0; i < maxPlayer; ++i)
        {
            transform.GetChild(i).gameObject.SetActive(true);
            if(myId == i)
            {
                transform.GetChild(i).gameObject.GetComponent<CharacterStatus>().SetAmIPlayer(true);
            }
        }
    }
}
