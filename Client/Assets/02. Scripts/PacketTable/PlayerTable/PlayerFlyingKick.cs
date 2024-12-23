// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.PlayerTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerFlyingKick : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerFlyingKick GetRootAsPlayerFlyingKick(ByteBuffer _bb) { return GetRootAsPlayerFlyingKick(_bb, new PlayerFlyingKick()); }
  public static PlayerFlyingKick GetRootAsPlayerFlyingKick(ByteBuffer _bb, PlayerFlyingKick obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerFlyingKick __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public PacketTable.PlayerTable.PlayerPos? PlayerInfo { get { int o = __p.__offset(4); return o != 0 ? (PacketTable.PlayerTable.PlayerPos?)(new PacketTable.PlayerTable.PlayerPos()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }

  public static Offset<PacketTable.PlayerTable.PlayerFlyingKick> CreatePlayerFlyingKick(FlatBufferBuilder builder,
      Offset<PacketTable.PlayerTable.PlayerPos> player_infoOffset = default(Offset<PacketTable.PlayerTable.PlayerPos>)) {
    builder.StartTable(1);
    PlayerFlyingKick.AddPlayerInfo(builder, player_infoOffset);
    return PlayerFlyingKick.EndPlayerFlyingKick(builder);
  }

  public static void StartPlayerFlyingKick(FlatBufferBuilder builder) { builder.StartTable(1); }
  public static void AddPlayerInfo(FlatBufferBuilder builder, Offset<PacketTable.PlayerTable.PlayerPos> playerInfoOffset) { builder.AddOffset(0, playerInfoOffset.Value, 0); }
  public static Offset<PacketTable.PlayerTable.PlayerFlyingKick> EndPlayerFlyingKick(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.PlayerTable.PlayerFlyingKick>(o);
  }
}


static public class PlayerFlyingKickVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyTable(tablePos, 4 /*PlayerInfo*/, PacketTable.PlayerTable.PlayerPosVerify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
