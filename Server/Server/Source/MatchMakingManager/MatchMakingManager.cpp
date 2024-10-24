#pragma once
#include "MatchMakingManager.h"
#include "../Server/Server.h"

MatchMakingManager::~MatchMakingManager()
{

}

void MatchMakingManager::MatchingComplete(int roomID, std::vector<class Player*>& players)
{
    Room* room = pServer->GetRooms()[roomID];

    room->SetPlayerCnt(players.size());

    // Player Add Into New Room
    for (Player* player : players) {
        player->GetSessionStateLock().lock();
        if (player->GetSessionState() == eSessionState::ST_MATCHWAITING) {
            // 인원수가 적은 팀에 배치
            int teamNum = 987654321;
            int minTeammateCnt = 987654321;
            for (auto& pair : room->GetTeams()) {
                int key = pair.first;
                Team& currTeam = pair.second;

                int teammateCnt = currTeam.GetMembers().size();

                if (teammateCnt < minTeammateCnt || (teammateCnt == minTeammateCnt && key < teamNum)) {
                    teamNum = key;
                    minTeammateCnt = teammateCnt;
                }
            }

            player->SetTeam(teamNum);
            player->ChangeCharacterType(pServer, eCharacterType::CT_TEST);
            player->SetHP(player->GetCharacterStat().hp);
            player->SetStamina(player->GetCharacterStat().stamina);

            int sessionID = player->GetSessionID();
            bool AddPlayerOk = room->AddPlayer(player);
            if (AddPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                if (room->GetHostID() == INVALIDKEY) {
                    room->SetHost(player->GetInGameID());
                }

                if (player->GetIsBot() == true) {
                }
                else {
                    pServer->GetPacketSender()->SendGameMatchingResponse(sessionID);
                    COUT << "Matched - " << sessionID << ENDL;
                }
            }
        }
        player->GetSessionStateLock().unlock();
    }

    pServer->GetPacketSender()->SendPlayerAdd(roomID);
}
