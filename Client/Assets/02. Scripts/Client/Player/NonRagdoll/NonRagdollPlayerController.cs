using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using JetBrains.Annotations;

public class NonRagdollPlayerController : MonoBehaviour
{
    private int maxPlayerNum = 6;

    public GameObject podiums;

    public void SetPlayer()
    {
        int winningTeam = Managers.Game.GetWinningTeam();

        int playerCount = 0;

        //이긴 팀이 있는 경우
        if (winningTeam != 3)
        {
            int winnerCount = 0;
            int loserCount = 0;

            GameObject winnerPositions = null;
            GameObject loserPositions = null;

            switch (Managers.Game.gameMode)
            {
                case eGameMode.FITH_Team_Battle_6:
                    {
                        podiums.transform.GetChild(2).gameObject.SetActive(true);
                        winnerPositions = podiums.transform.GetChild(2).transform.GetChild(0).gameObject;
                        loserPositions = podiums.transform.GetChild(2).transform.GetChild(1).gameObject;
                    }
                    break;
                case eGameMode.FITH_Team_Battle_4:
                    {
                        podiums.transform.GetChild(1).gameObject.SetActive(true);
                        winnerPositions = podiums.transform.GetChild(1).transform.GetChild(0).gameObject;
                        loserPositions = podiums.transform.GetChild(1).transform.GetChild(1).gameObject;
                    }
                    break;
                case eGameMode.FITH_Indiv_Battle_3:
                case eGameMode.FITH_Indiv_Battle_2:
                    {
                        podiums.transform.GetChild(0).gameObject.SetActive(true);
                        winnerPositions = podiums.transform.GetChild(0).transform.GetChild(0).gameObject;
                        loserPositions = podiums.transform.GetChild(0).transform.GetChild(1).gameObject;
                    }
                    break;
                default:
                    {
                        Debug.Log("ERROR!!! SetPlayer(): Wrong GameMode !!!");
                    }
                    break;
            }

            for (int i = 0; i < maxPlayerNum; ++i)
            {
                GameObject tPlayer = Managers.Player.FindPlayerById(i);
                if (tPlayer.activeSelf == true)
                {
                    int teamNumber = tPlayer.GetComponent<CharacterStatus>().GetTeamNumber();
                    GameObject rPlayer = this.transform.GetChild(playerCount).gameObject;
                    rPlayer.SetActive(true);

                    // 스킨 적용
                    List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                    foreach (int itemCode in itemList)
                    {
                        rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                    }

                    // 이긴 팀 플레이어면
                    if (winningTeam == teamNumber)
                    {
                        rPlayer.transform.position = winnerPositions.transform.GetChild(winnerCount).transform.position;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                        winnerCount++;
                    }
                    // 진 팀 플레이어면
                    else
                    {
                        rPlayer.transform.position = loserPositions.transform.GetChild(loserCount).transform.position;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetLoseAnimation();
                        loserCount++;
                    }
                    playerCount++;
                }
            }
        }
        //비긴 경우
        else
        {

        }
    }
    public void ResetPlayer()
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            GameObject tPlayer = this.transform.GetChild(i).gameObject;
            tPlayer.GetComponent<NonRagdollPlayerAnimationController>().ResetAnimation();
            tPlayer.SetActive(false);
        }
    }
}
