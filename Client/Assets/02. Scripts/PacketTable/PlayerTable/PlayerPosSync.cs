// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.PlayerTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerPosSync : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerPosSync GetRootAsPlayerPosSync(ByteBuffer _bb) { return GetRootAsPlayerPosSync(_bb, new PlayerPosSync()); }
  public static PlayerPosSync GetRootAsPlayerPosSync(ByteBuffer _bb, PlayerPosSync obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerPosSync __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Id { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public PacketTable.PlayerTable.Vec3? Pos { get { int o = __p.__offset(6); return o != 0 ? (PacketTable.PlayerTable.Vec3?)(new PacketTable.PlayerTable.Vec3()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public PacketTable.PlayerTable.Vec3? Direction { get { int o = __p.__offset(8); return o != 0 ? (PacketTable.PlayerTable.Vec3?)(new PacketTable.PlayerTable.Vec3()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }

  public static Offset<PacketTable.PlayerTable.PlayerPosSync> CreatePlayerPosSync(FlatBufferBuilder builder,
      int id = 0,
      Offset<PacketTable.PlayerTable.Vec3> posOffset = default(Offset<PacketTable.PlayerTable.Vec3>),
      Offset<PacketTable.PlayerTable.Vec3> directionOffset = default(Offset<PacketTable.PlayerTable.Vec3>)) {
    builder.StartTable(3);
    PlayerPosSync.AddDirection(builder, directionOffset);
    PlayerPosSync.AddPos(builder, posOffset);
    PlayerPosSync.AddId(builder, id);
    return PlayerPosSync.EndPlayerPosSync(builder);
  }

  public static void StartPlayerPosSync(FlatBufferBuilder builder) { builder.StartTable(3); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(0, id, 0); }
  public static void AddPos(FlatBufferBuilder builder, Offset<PacketTable.PlayerTable.Vec3> posOffset) { builder.AddOffset(1, posOffset.Value, 0); }
  public static void AddDirection(FlatBufferBuilder builder, Offset<PacketTable.PlayerTable.Vec3> directionOffset) { builder.AddOffset(2, directionOffset.Value, 0); }
  public static Offset<PacketTable.PlayerTable.PlayerPosSync> EndPlayerPosSync(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.PlayerTable.PlayerPosSync>(o);
  }
}


static public class PlayerPosSyncVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyTable(tablePos, 6 /*Pos*/, PacketTable.PlayerTable.Vec3Verify.Verify, false)
      && verifier.VerifyTable(tablePos, 8 /*Direction*/, PacketTable.PlayerTable.Vec3Verify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}