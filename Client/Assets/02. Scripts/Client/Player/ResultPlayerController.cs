using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResultPlayerController : MonoBehaviour
{
    // TODO: �߰� - ��������� - ���� ������ ���� �� �� �ֵ��� ����

    private int maxPlayerNum = 6;
    public void SetPlayer()
    {
        int winningTeam = Managers.Game.GetWinningTeam();

        // TODO: �߰� - ��� - ��庰 �÷��̾� ��ġ ����
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

                // �̱� �� �÷��̾��
                if (winningTeam == teamNumber)
                {
                    Debug.Log("Win");
                    rPlayer.transform.position = winnerOffset;
                    winnerOffset.x += winnerXInterval;
                    rPlayer.GetComponent<ResultAnimationController>().SetWinAnimation();
                }
                // �� �� �÷��̾��
                else
                {
                    Debug.Log("Lose");
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
