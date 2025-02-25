// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.ObjectTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct BombDrop : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static BombDrop GetRootAsBombDrop(ByteBuffer _bb) { return GetRootAsBombDrop(_bb, new BombDrop()); }
  public static BombDrop GetRootAsBombDrop(ByteBuffer _bb, BombDrop obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public BombDrop __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int PlayerId { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public PacketTable.ObjectTable.BombPosition? BombInfo { get { int o = __p.__offset(6); return o != 0 ? (PacketTable.ObjectTable.BombPosition?)(new PacketTable.ObjectTable.BombPosition()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }

  public static Offset<PacketTable.ObjectTable.BombDrop> CreateBombDrop(FlatBufferBuilder builder,
      int player_id = 0,
      Offset<PacketTable.ObjectTable.BombPosition> bomb_infoOffset = default(Offset<PacketTable.ObjectTable.BombPosition>)) {
    builder.StartTable(2);
    BombDrop.AddBombInfo(builder, bomb_infoOffset);
    BombDrop.AddPlayerId(builder, player_id);
    return BombDrop.EndBombDrop(builder);
  }

  public static void StartBombDrop(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddPlayerId(FlatBufferBuilder builder, int playerId) { builder.AddInt(0, playerId, 0); }
  public static void AddBombInfo(FlatBufferBuilder builder, Offset<PacketTable.ObjectTable.BombPosition> bombInfoOffset) { builder.AddOffset(1, bombInfoOffset.Value, 0); }
  public static Offset<PacketTable.ObjectTable.BombDrop> EndBombDrop(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.ObjectTable.BombDrop>(o);
  }
}


static public class BombDropVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*PlayerId*/, 4 /*int*/, 4, false)
      && verifier.VerifyTable(tablePos, 6 /*BombInfo*/, PacketTable.ObjectTable.BombPositionVerify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
