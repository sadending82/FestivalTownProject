// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_BLOCK_PACKETTABLE_OBJECTTABLE_H_
#define FLATBUFFERS_GENERATED_BLOCK_PACKETTABLE_OBJECTTABLE_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "Utilities_generated.h"

namespace PacketTable {
namespace ObjectTable {

struct BlockPosition;
struct BlockPositionBuilder;

struct BlockDrop;
struct BlockDropBuilder;

struct BlockPosition FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef BlockPositionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POS = 4,
    VT_ID = 6
  };
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           verifier.EndTable();
  }
};

struct BlockPositionBuilder {
  typedef BlockPosition Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(BlockPosition::VT_POS, pos);
  }
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(BlockPosition::VT_ID, id, 0);
  }
  explicit BlockPositionBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<BlockPosition> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<BlockPosition>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<BlockPosition> CreateBlockPosition(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    int32_t id = 0) {
  BlockPositionBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_pos(pos);
  return builder_.Finish();
}

struct BlockDrop FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef BlockDropBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POS = 4,
    VT_ID = 6
  };
  const PacketTable::UtilitiesTable::Vec2i *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec2i *>(VT_POS);
  }
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           verifier.EndTable();
  }
};

struct BlockDropBuilder {
  typedef BlockDrop Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec2i> pos) {
    fbb_.AddOffset(BlockDrop::VT_POS, pos);
  }
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(BlockDrop::VT_ID, id, 0);
  }
  explicit BlockDropBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<BlockDrop> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<BlockDrop>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<BlockDrop> CreateBlockDrop(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec2i> pos = 0,
    int32_t id = 0) {
  BlockDropBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_pos(pos);
  return builder_.Finish();
}

}  // namespace ObjectTable
}  // namespace PacketTable

#endif  // FLATBUFFERS_GENERATED_BLOCK_PACKETTABLE_OBJECTTABLE_H_