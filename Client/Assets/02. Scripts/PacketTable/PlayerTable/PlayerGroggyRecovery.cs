// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.PlayerTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerGroggyRecovery : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerGroggyRecovery GetRootAsPlayerGroggyRecovery(ByteBuffer _bb) { return GetRootAsPlayerGroggyRecovery(_bb, new PlayerGroggyRecovery()); }
  public static PlayerGroggyRecovery GetRootAsPlayerGroggyRecovery(ByteBuffer _bb, PlayerGroggyRecovery obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerGroggyRecovery __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Id { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int RecoveryValue { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.PlayerTable.PlayerGroggyRecovery> CreatePlayerGroggyRecovery(FlatBufferBuilder builder,
      int id = 0,
      int recovery_value = 0) {
    builder.StartTable(2);
    PlayerGroggyRecovery.AddRecoveryValue(builder, recovery_value);
    PlayerGroggyRecovery.AddId(builder, id);
    return PlayerGroggyRecovery.EndPlayerGroggyRecovery(builder);
  }

  public static void StartPlayerGroggyRecovery(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(0, id, 0); }
  public static void AddRecoveryValue(FlatBufferBuilder builder, int recoveryValue) { builder.AddInt(1, recoveryValue, 0); }
  public static Offset<PacketTable.PlayerTable.PlayerGroggyRecovery> EndPlayerGroggyRecovery(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.PlayerTable.PlayerGroggyRecovery>(o);
  }
}


static public class PlayerGroggyRecoveryVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 6 /*RecoveryValue*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
