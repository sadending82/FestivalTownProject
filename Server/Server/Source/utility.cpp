#pragma once
#include "utility.h"
#include "Object/Player.h"
#include "PacketManager/flatbuffer/FlatBufferManager.h"
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

std::string wstringToString(const std::wstring& input)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
	std::string utf8_string(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, &utf8_string[0], size, NULL, NULL);
	return utf8_string;
}

sCharacterCustomizing DeserializationCharacterCustomizing(const uint8_t* data, const int size)
{
	sCharacterCustomizing sCustomizing;
	flatbuffers::Verifier verifier(data, size);
	if (verifier.VerifyBuffer<PacketTable::UtilitiesTable::CharacterCustomizing>(nullptr)) {

		const PacketTable::UtilitiesTable::CharacterCustomizing* fCustomizing = flatbuffers::GetRoot<PacketTable::UtilitiesTable::CharacterCustomizing>(data);

		auto items = fCustomizing->customizing_items();

		for (const auto& item : *items) {
			EquippedItem equippedItem;
			equippedItem.item_UID = item->item_uid();
			equippedItem.itemCode= item->item_code();
			sCustomizing.SetItem((CustomizingItemType)item->type(), equippedItem);
		}
	}
	else {
		return sCharacterCustomizing();
	}

	return sCustomizing;
}

std::vector<uint8_t> SerializationCharacterCustomizing(sCharacterCustomizing characterCustomizing)
{
	flatbuffers::FlatBufferBuilder Builder;
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::CustomizingItem>> item_vecter;

	for (auto& pair : characterCustomizing.customizingItems) {
		int type = pair.first;
		EquippedItem equippedItem = pair.second;

		auto fCustomizingItem = PacketTable::UtilitiesTable::CreateCustomizingItem(Builder, type, equippedItem.item_UID, equippedItem.itemCode);

		item_vecter.push_back(fCustomizingItem);
	}

	Builder.Finish(PacketTable::UtilitiesTable::CreateCharacterCustomizing(Builder, Builder.CreateVector(item_vecter)));

	std::vector<uint8_t> result(Builder.GetSize());
	memcpy(result.data(), Builder.GetBufferPointer(), Builder.GetSize());

	return result;
}
