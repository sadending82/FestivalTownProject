// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.GameTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct Game : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static Game GetRootAsGame(ByteBuffer _bb) { return GetRootAsGame(_bb, new Game()); }
  public static Game GetRootAsGame(ByteBuffer _bb, Game obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyGame(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, GameVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public Game __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }


  public static void StartGame(FlatBufferBuilder builder) { builder.StartTable(0); }
  public static Offset<PacketTable.GameTable.Game> EndGame(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.GameTable.Game>(o);
  }
  public static void FinishGameBuffer(FlatBufferBuilder builder, Offset<PacketTable.GameTable.Game> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedGameBuffer(FlatBufferBuilder builder, Offset<PacketTable.GameTable.Game> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class GameVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
