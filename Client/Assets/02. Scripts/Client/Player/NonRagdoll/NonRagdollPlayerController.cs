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
        Debug.Log("WinningTeam: " + winningTeam);

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
                        rPlayer.transform.rotation = winnerPositions.transform.GetChild(winnerCount).transform.rotation;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                        winnerCount++;
                    }
                    // 진 팀 플레이어면
                    else
                    {
                        rPlayer.transform.position = loserPositions.transform.GetChild(loserCount).transform.position;
                        rPlayer.transform.rotation = loserPositions.transform.GetChild(loserCount).transform.rotation;
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
            int leftCount = 0;
            int rightCount = 0;
            int middleCount = 0;

            int leftTeamNumber = -1;

            GameObject leftPositions = null;
            GameObject rightPositions = null;
            GameObject middlePositions = null;

            podiums.transform.GetChild(3).gameObject.SetActive(true);
            switch (Managers.Game.gameMode)
            {
                case eGameMode.FITH_Team_Battle_6:
                case eGameMode.FITH_Team_Battle_4:
                case eGameMode.FITH_Indiv_Battle_2:
                    {
                        leftPositions = podiums.transform.GetChild(3).transform.GetChild(0).gameObject;
                        rightPositions = podiums.transform.GetChild(3).transform.GetChild(1).gameObject;
                    }
                    break;
                case eGameMode.FITH_Indiv_Battle_3:
                    {
                        middlePositions = podiums.transform.GetChild(3).transform.GetChild(2).gameObject;
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
                    if(leftTeamNumber == -1)
                    {
                        leftTeamNumber = teamNumber;
                    }

                    //스킨 적용
                    List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                    foreach (int itemCode in itemList)
                    {
                        rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                    }
                    //애니메이션 적용
                    rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetDrawAnimation();

                    switch (Managers.Game.gameMode)
                    {
                        case eGameMode.FITH_Team_Battle_6:
                        case eGameMode.FITH_Team_Battle_4:
                        case eGameMode.FITH_Indiv_Battle_2:
                            {
                                //왼쪽팀 플레이어면
                                if (leftTeamNumber == teamNumber)
                                {
                                    rPlayer.transform.position = leftPositions.transform.GetChild(leftCount).transform.position;
                                    rPlayer.transform.rotation = leftPositions.transform.GetChild(leftCount).transform.rotation;
                                    leftCount++;
                                }
                                //오른쪽팀 플레이어면
                                else
                                {
                                    rPlayer.transform.position = rightPositions.transform.GetChild(rightCount).transform.position;
                                    rPlayer.transform.rotation = rightPositions.transform.GetChild(rightCount).transform.rotation;
                                    rightCount++;
                                }
                            }
                            break;
                        case eGameMode.FITH_Indiv_Battle_3:
                            {
                                rPlayer.transform.position = middlePositions.transform.GetChild(middleCount).transform.position;
                                rPlayer.transform.rotation = middlePositions.transform.GetChild(middleCount).transform.rotation;
                                middleCount++;
                            }
                            break;
                        default:
                            {
                            }
                            break;
                    }
                    playerCount++;
                }
            }
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
