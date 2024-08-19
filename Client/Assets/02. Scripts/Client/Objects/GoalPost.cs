using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoalPost : MonoBehaviour
{
    public int teamNumber;

    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.tag == "Bomb")
        {
            Bomb targetBomb = other.GetComponent<Bomb>();
            int playerId = targetBomb.GetLastPickUpPlayerId();

            Managers.Player.transform.GetChild(playerId).GetComponent<PlayerController>().SendToServerGoalTeamNumber(targetBomb.GetId(), teamNumber);
            targetBomb.Boom();
        }
    }
}
