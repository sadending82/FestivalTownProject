// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.PlayerTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerDropBomb : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerDropBomb GetRootAsPlayerDropBomb(ByteBuffer _bb) { return GetRootAsPlayerDropBomb(_bb, new PlayerDropBomb()); }
  public static PlayerDropBomb GetRootAsPlayerDropBomb(ByteBuffer _bb, PlayerDropBomb obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerDropBomb __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Id { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public PacketTable.UtilitiesTable.Vec3f? Pos { get { int o = __p.__offset(6); return o != 0 ? (PacketTable.UtilitiesTable.Vec3f?)(new PacketTable.UtilitiesTable.Vec3f()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public int BombId { get { int o = __p.__offset(8); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.PlayerTable.PlayerDropBomb> CreatePlayerDropBomb(FlatBufferBuilder builder,
      int id = 0,
      Offset<PacketTable.UtilitiesTable.Vec3f> posOffset = default(Offset<PacketTable.UtilitiesTable.Vec3f>),
      int bomb_id = 0) {
    builder.StartTable(3);
    PlayerDropBomb.AddBombId(builder, bomb_id);
    PlayerDropBomb.AddPos(builder, posOffset);
    PlayerDropBomb.AddId(builder, id);
    return PlayerDropBomb.EndPlayerDropBomb(builder);
  }

  public static void StartPlayerDropBomb(FlatBufferBuilder builder) { builder.StartTable(3); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(0, id, 0); }
  public static void AddPos(FlatBufferBuilder builder, Offset<PacketTable.UtilitiesTable.Vec3f> posOffset) { builder.AddOffset(1, posOffset.Value, 0); }
  public static void AddBombId(FlatBufferBuilder builder, int bombId) { builder.AddInt(2, bombId, 0); }
  public static Offset<PacketTable.PlayerTable.PlayerDropBomb> EndPlayerDropBomb(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.PlayerTable.PlayerDropBomb>(o);
  }
}


static public class PlayerDropBombVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyTable(tablePos, 6 /*Pos*/, PacketTable.UtilitiesTable.Vec3fVerify.Verify, false)
      && verifier.VerifyField(tablePos, 8 /*BombId*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}