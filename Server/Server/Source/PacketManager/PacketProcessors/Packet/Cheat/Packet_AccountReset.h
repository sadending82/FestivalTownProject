#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::CheatTable;

class Packet_AccountReset : public PacketProcessor {

public:
	Packet_AccountReset(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<AccountReset>(nullptr)) {
				if (pServer->GetMode() != SERVER_MODE::TEST) {
					return;
				}
				const AccountReset* read = flatbuffers::GetRoot<AccountReset>(data);
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr) {
					return;
				}
				
				int uid = player->GetUID();
				DB* db = pServer->GetDB();

				db->DeleteUserInfo(uid);
				int new_uid = db->InsertNewUser(player->GetAccountID().c_str(), player->GetNickName().c_str());

				db->DeleteUserGameRecords(uid);
				db->InsertUserGameRecords(new_uid);

				db->DeleteUserItemAll(uid);
				db->DeleteUserAttendanceAll(uid);


				pServer->Disconnect(key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_AccountReset ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};