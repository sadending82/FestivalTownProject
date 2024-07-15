#include "PacketManager.h"

void PacketManager::Init()
{

}

void PacketManager::ProcessPacket(int key, int type, const uint8_t* data, const int size)
{
	/*flatbuffers::Verifier verifier(data, size);
	if (VerifytestBuffer(verifier)) {

		const test* read_test = Gettest(data);

		DEBUGMSGONEPARAM("Chat -> %s\n", read_test->chat()->c_str());
		DEBUGMSGONEPARAM("Number -> %d\n", read_test->number());
	}*/
}

vector<uint8_t> PacketManager::MakeBuffer(char type, const uint8_t* data, const int size)
{
	HEADER h;
	h.size = size;
	h.type = type;
	std::vector<uint8_t> buf(sizeof(HEADER) + size);
	memcpy(buf.data(), &h, sizeof(HEADER));
	memcpy(buf.data() + sizeof(HEADER), data, size);

	return buf;
}
