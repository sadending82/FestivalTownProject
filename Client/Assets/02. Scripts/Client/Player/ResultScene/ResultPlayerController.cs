using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResultPlayerController : MonoBehaviour
{
    // TODO: 추가 - 착용아이템 - 착용 아이템 세팅 할 수 있도록 수정

    private int maxPlayerNum = 6;
    public void SetPlayer()
    {
        int winningTeam = Managers.Game.GetWinningTeam();

        // TODO: 추가 - 모드 - 모드별 플레이어 위치 수정
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
                    rPlayer.transform.position = winnerOffset;
                    winnerOffset.x += winnerXInterval;
                    rPlayer.GetComponent<ResultAnimationController>().SetWinAnimation();
                }
                // 진 팀 플레이어면
                else
                {
                    rPlayer.transform.position = loserOffset;
                    loserOffset.x += loserXInterval;
                    rPlayer.GetComponent <ResultAnimationController>().SetLoseAnimation();
                }
                playerCount++;
            }
        }
    }
    public void ResetPlayer()
    {
        for (int i = 0; i < maxPlayerNum; ++i)
        {
            GameObject tPlayer = this.transform.GetChild(i).gameObject;
            tPlayer.GetComponent<ResultAnimationController>().ResetAnimation();
            tPlayer.SetActive(false);
        }
    }
}
