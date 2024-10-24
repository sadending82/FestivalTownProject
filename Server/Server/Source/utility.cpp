#include "utility.h"
#include "Object/Player.h"
#include <vector>
#include <cmath>

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size)
{
	HEADER h;
	h.flatBufferSize = size;
	h.type = type;

	std::vector<uint8_t> buf(sizeof(HEADER) + size);
	memcpy(buf.data(), &h, sizeof(HEADER));
	memcpy(buf.data() + sizeof(HEADER), data, size);

	return buf;
}

Vector3f ConvertVec3fToVec2i(Vector3f vec)
{
	float x = vec.x;
	float z = vec.z;

	return Vector3f(std::trunc(x / BLOCKSIZE), 0, std::trunc(z / BLOCKSIZE));
}

Vector3f ConvertVec2iToVec3f(int x, int z)
{
	Vector3f pos;

	pos.x = x * BLOCKSIZE + (BLOCKSIZE / 2);
	pos.y = 0;
	pos.z = z * BLOCKSIZE + (BLOCKSIZE / 2);

	return pos;
}

bool MatchingCompare::operator()(Player* a, Player* b) const {
    return a->GetMatchingRequestTime() < b->GetMatchingRequestTime();
}

GameMode CulculateGameMode(int playerCount)
{
    switch (playerCount) {
    case 2: {
        return GameMode::FITH_Indiv_Battle_2;
    }break;
    case 3: {
        return GameMode::FITH_Indiv_Battle_3;
    }break;
    case 4: {
        return GameMode::FITH_Team_Battle_4;
    }break;
    case 5: {
        return GameMode::FITH_Indiv_Battle_3;
    }break;
    case 6: {
        return GameMode::FITH_Team_Battle_6;
    }break;
    case 7: {
        return GameMode::FITH_Team_Battle_4;
    }break;
          // over 8
    default: {
        return GameMode::FITH_Team_Battle_6;
    }break;
    }
}