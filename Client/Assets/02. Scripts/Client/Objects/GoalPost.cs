using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoalPost : MonoBehaviour
{ 
    [SerializeField] private int teamNumber;

    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.tag == "Bomb")
        {
            Bomb targetBomb = other.GetComponent<Bomb>();
            if (targetBomb == null) return;

            if (Managers.Player.GetIsHost() == true)
            {
                Managers.Game.SendToServerGoalTeamNumber(targetBomb.GetId(), teamNumber, targetBomb.GetLastPickUpPlayerId());
            }
            targetBomb.Boom();
        }
    }
    public void SetTeamNumber(int teamNumber)
    {
        this.teamNumber = teamNumber;
    }
}
