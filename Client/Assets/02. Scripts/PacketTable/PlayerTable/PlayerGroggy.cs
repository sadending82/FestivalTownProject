// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.PlayerTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerGroggy : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerGroggy GetRootAsPlayerGroggy(ByteBuffer _bb) { return GetRootAsPlayerGroggy(_bb, new PlayerGroggy()); }
  public static PlayerGroggy GetRootAsPlayerGroggy(ByteBuffer _bb, PlayerGroggy obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerGroggy __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Id { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int Hp { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.PlayerTable.PlayerGroggy> CreatePlayerGroggy(FlatBufferBuilder builder,
      int id = 0,
      int hp = 0) {
    builder.StartTable(2);
    PlayerGroggy.AddHp(builder, hp);
    PlayerGroggy.AddId(builder, id);
    return PlayerGroggy.EndPlayerGroggy(builder);
  }

  public static void StartPlayerGroggy(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(0, id, 0); }
  public static void AddHp(FlatBufferBuilder builder, int hp) { builder.AddInt(1, hp, 0); }
  public static Offset<PacketTable.PlayerTable.PlayerGroggy> EndPlayerGroggy(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.PlayerTable.PlayerGroggy>(o);
  }
}


static public class PlayerGroggyVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 6 /*Hp*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
