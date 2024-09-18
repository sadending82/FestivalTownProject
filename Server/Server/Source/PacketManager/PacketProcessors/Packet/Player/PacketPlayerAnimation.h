#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerAnimation : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerAnimation>(nullptr)) {

			const PlayerAnimation* read = flatbuffers::GetRoot<PlayerAnimation>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}

			UpperBodyAnimationState animation = (UpperBodyAnimationState)read->animation();
			if (UpperBodyAnimationState::ATTACK <= animation && animation <= UpperBodyAnimationState::WEAPONATTACK) {
				if (player->GetWeapon() != nullptr) {
					int weaponType = player->GetWeapon()->GetType();
					int staminaConsume = pServer->GetTableManager()->GetWeaponStats()[weaponType]->Weapon_StaminaConsume;
					if (player->GetStamina() - staminaConsume <= 0) {
						return;
					}
					player->ReduceStamina(staminaConsume);
				}
			}

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_ANIMATION, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};