using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;

public class NonRagdollPlayerController : MonoBehaviour
{
    // TODO: �߰� - ��������� - ���� ������ ���� �� �� �ֵ��� ����

    private int maxPlayerNum = 6;
    public void SetPlayer()
    {
        int winningTeam = Managers.Game.GetWinningTeam();

        // TODO: �߰� - ��� - ��庰 �÷��̾� ��ġ ����
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

                    // �̱� �� �÷��̾��
                    if (winningTeam == teamNumber)
                    {
                        // ��Ų ����
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = winnerOffset;
                        winnerOffset.x += winnerXInterval;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                    }
                    // �� �� �÷��̾��
                    else
                    {
                        // ��Ų ����
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

                    // �̱� �� �÷��̾��
                    if (winningTeam == teamNumber)
                    {
                        // ��Ų ����
                        List<int> itemList = Managers.Game.GetCharacterCustomizingById(i);
                        foreach (int itemCode in itemList)
                        {
                            rPlayer.GetComponent<NonRagdollPlayerState>().ChangeCustomizing(itemCode);
                        }

                        rPlayer.transform.position = winnerOffset;
                        rPlayer.GetComponent<NonRagdollPlayerAnimationController>().SetWinAnimation();
                    }
                    // �� �� �÷��̾��
                    else
                    {
                        // ��Ų ����
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
