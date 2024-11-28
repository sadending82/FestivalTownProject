using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;

public class NonRagdollPlayerController : MonoBehaviour
{
    // TODO: 추가 - 착용아이템 - 착용 아이템 세팅 할 수 있도록 수정

    private int maxPlayerNum = 6;
    public void SetPlayer()
    {
        int winningTeam = Managers.Game.GetWinningTeam();

        // TODO: 추가 - 모드 - 모드별 플레이어 위치 수정
        if (Managers.Game.gameMode == eGameMode.FITH_Team_Battle_6)
        {
            Vector3 winnerOffset = new Vector3(-2, 75.3f, 13);
            float winnerXInterval = 2f;
            Vector3 loserOffset = new Vector3(-2, 73.3f, 11);
            float loserXInterval = 2f;

            int playerCount = 0;

            for (int i = 0; i < maxPlayerNum; ++i)
            {
                GameObject tPlayer = Managers.Player.FindPlayerById(i);
                if (tPlayer.activeSelf == true)
                {
                    int teamNumber = tPlayer.GetComponent<CharacterStatus>().GetTeamNumber();
                    GameObject rPlayer = this.transform.GetChild(playerCount).gameObject;
                    rPlayer.SetActive(true);

                    // 이긴 팀 플레이어면
                    if (winningTeam == teamNumber)
                    {
                        // 스킨 적용
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = winnerOffset;
                        winnerOffset.x += winnerXInterval;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                    }
                    // 진 팀 플레이어면
                    else
                    {
                        // 스킨 적용
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = loserOffset;
                        loserOffset.x += loserXInterval;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetLoseAnimation();
                    }
                    playerCount++;
                }
            }
        }
        else if(Managers.Game.gameMode == eGameMode.FITH_Indiv_Battle_3)
        {
            Vector3 winnerOffset = new Vector3(0, 75.3f, 13);
            Vector3 loserOffset = new Vector3(-1, 73.3f, 11);
            float loserXInterval = 2f;

            int playerCount = 0;
            for (int i = 0; i < maxPlayerNum; ++i)
            {
                GameObject tPlayer = Managers.Player.FindPlayerById(i);
                if (tPlayer.activeSelf == true)
                {
                    int teamNumber = tPlayer.GetComponent<CharacterStatus>().GetTeamNumber();
                    GameObject rPlayer = this.transform.GetChild(playerCount).gameObject;
                    rPlayer.SetActive(true);

                    // 이긴 팀 플레이어면
                    if (winningTeam == teamNumber)
                    {
                        // 스킨 적용
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = winnerOffset;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                    }
                    // 진 팀 플레이어면
                    else
                    {
                        // 스킨 적용
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = loserOffset;
                        loserOffset.x += loserXInterval;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetLoseAnimation();
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
