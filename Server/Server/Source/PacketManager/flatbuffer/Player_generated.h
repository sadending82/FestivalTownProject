// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PLAYER_PACKETTABLE_PLAYERTABLE_H_
#define FLATBUFFERS_GENERATED_PLAYER_PACKETTABLE_PLAYERTABLE_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "Utilities_generated.h"

namespace PacketTable {
namespace PlayerTable {

struct PlayerPos;
struct PlayerPosBuilder;

struct PlayerPosSync;
struct PlayerPosSyncBuilder;

struct PlayerAdd;
struct PlayerAddBuilder;

struct PlayerMove;
struct PlayerMoveBuilder;

struct PlayerStop;
struct PlayerStopBuilder;

struct PlayerGrabBomb;
struct PlayerGrabBombBuilder;

struct PlayerThrowBomb;
struct PlayerThrowBombBuilder;

struct PlayerAnimation;
struct PlayerAnimationBuilder;

struct PlayerDead;
struct PlayerDeadBuilder;

struct PlayerDamageReceive;
struct PlayerDamageReceiveBuilder;

struct PlayerCalculatedDamage;
struct PlayerCalculatedDamageBuilder;

struct PlayerRespawn;
struct PlayerRespawnBuilder;

struct PlayerPos FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerPosBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           verifier.EndTable();
  }
};

struct PlayerPosBuilder {
  typedef PlayerPos Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerPos::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerPos::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerPos::VT_DIRECTION, direction);
  }
  explicit PlayerPosBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerPos> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerPos>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerPos> CreatePlayerPos(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0) {
  PlayerPosBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerPosSync FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerPosSyncBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           verifier.EndTable();
  }
};

struct PlayerPosSyncBuilder {
  typedef PlayerPosSync Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerPosSync::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerPosSync::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerPosSync::VT_DIRECTION, direction);
  }
  explicit PlayerPosSyncBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerPosSync> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerPosSync>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerPosSync> CreatePlayerPosSync(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0) {
  PlayerPosSyncBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerAdd FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerAddBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           verifier.EndTable();
  }
};

struct PlayerAddBuilder {
  typedef PlayerAdd Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerAdd::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerAdd::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerAdd::VT_DIRECTION, direction);
  }
  explicit PlayerAddBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerAdd> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerAdd>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerAdd> CreatePlayerAdd(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0) {
  PlayerAddBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerMove FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerMoveBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8,
    VT_STATE = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  int32_t state() const {
    return GetField<int32_t>(VT_STATE, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           VerifyField<int32_t>(verifier, VT_STATE, 4) &&
           verifier.EndTable();
  }
};

struct PlayerMoveBuilder {
  typedef PlayerMove Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerMove::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerMove::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerMove::VT_DIRECTION, direction);
  }
  void add_state(int32_t state) {
    fbb_.AddElement<int32_t>(PlayerMove::VT_STATE, state, 0);
  }
  explicit PlayerMoveBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerMove> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerMove>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerMove> CreatePlayerMove(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0,
    int32_t state = 0) {
  PlayerMoveBuilder builder_(_fbb);
  builder_.add_state(state);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerStop FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerStopBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8,
    VT_STATE = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  int32_t state() const {
    return GetField<int32_t>(VT_STATE, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           VerifyField<int32_t>(verifier, VT_STATE, 4) &&
           verifier.EndTable();
  }
};

struct PlayerStopBuilder {
  typedef PlayerStop Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerStop::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerStop::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerStop::VT_DIRECTION, direction);
  }
  void add_state(int32_t state) {
    fbb_.AddElement<int32_t>(PlayerStop::VT_STATE, state, 0);
  }
  explicit PlayerStopBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerStop> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerStop>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerStop> CreatePlayerStop(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0,
    int32_t state = 0) {
  PlayerStopBuilder builder_(_fbb);
  builder_.add_state(state);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerGrabBomb FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerGrabBombBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8,
    VT_BOMB_ID = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  int32_t bomb_id() const {
    return GetField<int32_t>(VT_BOMB_ID, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           VerifyField<int32_t>(verifier, VT_BOMB_ID, 4) &&
           verifier.EndTable();
  }
};

struct PlayerGrabBombBuilder {
  typedef PlayerGrabBomb Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerGrabBomb::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerGrabBomb::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerGrabBomb::VT_DIRECTION, direction);
  }
  void add_bomb_id(int32_t bomb_id) {
    fbb_.AddElement<int32_t>(PlayerGrabBomb::VT_BOMB_ID, bomb_id, 0);
  }
  explicit PlayerGrabBombBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerGrabBomb> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerGrabBomb>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerGrabBomb> CreatePlayerGrabBomb(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0,
    int32_t bomb_id = 0) {
  PlayerGrabBombBuilder builder_(_fbb);
  builder_.add_bomb_id(bomb_id);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerThrowBomb FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerThrowBombBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8,
    VT_BOMB_ID = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  int32_t bomb_id() const {
    return GetField<int32_t>(VT_BOMB_ID, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           VerifyField<int32_t>(verifier, VT_BOMB_ID, 4) &&
           verifier.EndTable();
  }
};

struct PlayerThrowBombBuilder {
  typedef PlayerThrowBomb Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerThrowBomb::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerThrowBomb::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerThrowBomb::VT_DIRECTION, direction);
  }
  void add_bomb_id(int32_t bomb_id) {
    fbb_.AddElement<int32_t>(PlayerThrowBomb::VT_BOMB_ID, bomb_id, 0);
  }
  explicit PlayerThrowBombBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerThrowBomb> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerThrowBomb>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerThrowBomb> CreatePlayerThrowBomb(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0,
    int32_t bomb_id = 0) {
  PlayerThrowBombBuilder builder_(_fbb);
  builder_.add_bomb_id(bomb_id);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerAnimation FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerAnimationBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8,
    VT_ANIMATION = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  int32_t animation() const {
    return GetField<int32_t>(VT_ANIMATION, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           VerifyField<int32_t>(verifier, VT_ANIMATION, 4) &&
           verifier.EndTable();
  }
};

struct PlayerAnimationBuilder {
  typedef PlayerAnimation Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerAnimation::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerAnimation::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerAnimation::VT_DIRECTION, direction);
  }
  void add_animation(int32_t animation) {
    fbb_.AddElement<int32_t>(PlayerAnimation::VT_ANIMATION, animation, 0);
  }
  explicit PlayerAnimationBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerAnimation> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerAnimation>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerAnimation> CreatePlayerAnimation(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0,
    int32_t animation = 0) {
  PlayerAnimationBuilder builder_(_fbb);
  builder_.add_animation(animation);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerDead FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerDeadBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POS = 6,
    VT_DIRECTION = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           verifier.EndTable();
  }
};

struct PlayerDeadBuilder {
  typedef PlayerDead Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerDead::VT_ID, id, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerDead::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerDead::VT_DIRECTION, direction);
  }
  explicit PlayerDeadBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerDead> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerDead>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerDead> CreatePlayerDead(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0) {
  PlayerDeadBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_id(id);
  return builder_.Finish();
}

struct PlayerDamageReceive FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerDamageReceiveBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TARGET_ID = 4,
    VT_ATTACKER_ID = 6,
    VT_WEAPON = 8,
    VT_ATTACK_TYPE = 10,
    VT_KNOCKBACK_DIRECTION = 12
  };
  int32_t target_id() const {
    return GetField<int32_t>(VT_TARGET_ID, 0);
  }
  int32_t attacker_id() const {
    return GetField<int32_t>(VT_ATTACKER_ID, 0);
  }
  int32_t weapon() const {
    return GetField<int32_t>(VT_WEAPON, 0);
  }
  int32_t attack_type() const {
    return GetField<int32_t>(VT_ATTACK_TYPE, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *knockback_direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_KNOCKBACK_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_TARGET_ID, 4) &&
           VerifyField<int32_t>(verifier, VT_ATTACKER_ID, 4) &&
           VerifyField<int32_t>(verifier, VT_WEAPON, 4) &&
           VerifyField<int32_t>(verifier, VT_ATTACK_TYPE, 4) &&
           VerifyOffset(verifier, VT_KNOCKBACK_DIRECTION) &&
           verifier.VerifyTable(knockback_direction()) &&
           verifier.EndTable();
  }
};

struct PlayerDamageReceiveBuilder {
  typedef PlayerDamageReceive Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_target_id(int32_t target_id) {
    fbb_.AddElement<int32_t>(PlayerDamageReceive::VT_TARGET_ID, target_id, 0);
  }
  void add_attacker_id(int32_t attacker_id) {
    fbb_.AddElement<int32_t>(PlayerDamageReceive::VT_ATTACKER_ID, attacker_id, 0);
  }
  void add_weapon(int32_t weapon) {
    fbb_.AddElement<int32_t>(PlayerDamageReceive::VT_WEAPON, weapon, 0);
  }
  void add_attack_type(int32_t attack_type) {
    fbb_.AddElement<int32_t>(PlayerDamageReceive::VT_ATTACK_TYPE, attack_type, 0);
  }
  void add_knockback_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> knockback_direction) {
    fbb_.AddOffset(PlayerDamageReceive::VT_KNOCKBACK_DIRECTION, knockback_direction);
  }
  explicit PlayerDamageReceiveBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerDamageReceive> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerDamageReceive>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerDamageReceive> CreatePlayerDamageReceive(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t target_id = 0,
    int32_t attacker_id = 0,
    int32_t weapon = 0,
    int32_t attack_type = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> knockback_direction = 0) {
  PlayerDamageReceiveBuilder builder_(_fbb);
  builder_.add_knockback_direction(knockback_direction);
  builder_.add_attack_type(attack_type);
  builder_.add_weapon(weapon);
  builder_.add_attacker_id(attacker_id);
  builder_.add_target_id(target_id);
  return builder_.Finish();
}

struct PlayerCalculatedDamage FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerCalculatedDamageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TARGET_ID = 4,
    VT_ATTACK_TYPE = 6,
    VT_HP = 8,
    VT_DAMAGE_AMOUNT = 10,
    VT_KNOCKBACK_DIRECTION = 12
  };
  int32_t target_id() const {
    return GetField<int32_t>(VT_TARGET_ID, 0);
  }
  int32_t attack_type() const {
    return GetField<int32_t>(VT_ATTACK_TYPE, 0);
  }
  int32_t hp() const {
    return GetField<int32_t>(VT_HP, 0);
  }
  int32_t damage_amount() const {
    return GetField<int32_t>(VT_DAMAGE_AMOUNT, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *knockback_direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_KNOCKBACK_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_TARGET_ID, 4) &&
           VerifyField<int32_t>(verifier, VT_ATTACK_TYPE, 4) &&
           VerifyField<int32_t>(verifier, VT_HP, 4) &&
           VerifyField<int32_t>(verifier, VT_DAMAGE_AMOUNT, 4) &&
           VerifyOffset(verifier, VT_KNOCKBACK_DIRECTION) &&
           verifier.VerifyTable(knockback_direction()) &&
           verifier.EndTable();
  }
};

struct PlayerCalculatedDamageBuilder {
  typedef PlayerCalculatedDamage Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_target_id(int32_t target_id) {
    fbb_.AddElement<int32_t>(PlayerCalculatedDamage::VT_TARGET_ID, target_id, 0);
  }
  void add_attack_type(int32_t attack_type) {
    fbb_.AddElement<int32_t>(PlayerCalculatedDamage::VT_ATTACK_TYPE, attack_type, 0);
  }
  void add_hp(int32_t hp) {
    fbb_.AddElement<int32_t>(PlayerCalculatedDamage::VT_HP, hp, 0);
  }
  void add_damage_amount(int32_t damage_amount) {
    fbb_.AddElement<int32_t>(PlayerCalculatedDamage::VT_DAMAGE_AMOUNT, damage_amount, 0);
  }
  void add_knockback_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> knockback_direction) {
    fbb_.AddOffset(PlayerCalculatedDamage::VT_KNOCKBACK_DIRECTION, knockback_direction);
  }
  explicit PlayerCalculatedDamageBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerCalculatedDamage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerCalculatedDamage>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerCalculatedDamage> CreatePlayerCalculatedDamage(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t target_id = 0,
    int32_t attack_type = 0,
    int32_t hp = 0,
    int32_t damage_amount = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> knockback_direction = 0) {
  PlayerCalculatedDamageBuilder builder_(_fbb);
  builder_.add_knockback_direction(knockback_direction);
  builder_.add_damage_amount(damage_amount);
  builder_.add_hp(hp);
  builder_.add_attack_type(attack_type);
  builder_.add_target_id(target_id);
  return builder_.Finish();
}

struct PlayerRespawn FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PlayerRespawnBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_HP = 6,
    VT_POS = 8,
    VT_DIRECTION = 10
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  int32_t hp() const {
    return GetField<int32_t>(VT_HP, 0);
  }
  const PacketTable::UtilitiesTable::Vec3f *pos() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_POS);
  }
  const PacketTable::UtilitiesTable::Vec3f *direction() const {
    return GetPointer<const PacketTable::UtilitiesTable::Vec3f *>(VT_DIRECTION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID, 4) &&
           VerifyField<int32_t>(verifier, VT_HP, 4) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           VerifyOffset(verifier, VT_DIRECTION) &&
           verifier.VerifyTable(direction()) &&
           verifier.EndTable();
  }
};

struct PlayerRespawnBuilder {
  typedef PlayerRespawn Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(PlayerRespawn::VT_ID, id, 0);
  }
  void add_hp(int32_t hp) {
    fbb_.AddElement<int32_t>(PlayerRespawn::VT_HP, hp, 0);
  }
  void add_pos(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos) {
    fbb_.AddOffset(PlayerRespawn::VT_POS, pos);
  }
  void add_direction(::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction) {
    fbb_.AddOffset(PlayerRespawn::VT_DIRECTION, direction);
  }
  explicit PlayerRespawnBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PlayerRespawn> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PlayerRespawn>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PlayerRespawn> CreatePlayerRespawn(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    int32_t hp = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> pos = 0,
    ::flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f> direction = 0) {
  PlayerRespawnBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_pos(pos);
  builder_.add_hp(hp);
  builder_.add_id(id);
  return builder_.Finish();
}

inline const PacketTable::PlayerTable::PlayerPos *GetPlayerPos(const void *buf) {
  return ::flatbuffers::GetRoot<PacketTable::PlayerTable::PlayerPos>(buf);
}

inline const PacketTable::PlayerTable::PlayerPos *GetSizePrefixedPlayerPos(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<PacketTable::PlayerTable::PlayerPos>(buf);
}

inline bool VerifyPlayerPosBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<PacketTable::PlayerTable::PlayerPos>(nullptr);
}

inline bool VerifySizePrefixedPlayerPosBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<PacketTable::PlayerTable::PlayerPos>(nullptr);
}

inline void FinishPlayerPosBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<PacketTable::PlayerTable::PlayerPos> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPlayerPosBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<PacketTable::PlayerTable::PlayerPos> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace PlayerTable
}  // namespace PacketTable

#endif  // FLATBUFFERS_GENERATED_PLAYER_PACKETTABLE_PLAYERTABLE_H_