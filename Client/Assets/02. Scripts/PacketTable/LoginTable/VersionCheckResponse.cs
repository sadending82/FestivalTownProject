// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.LoginTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct VersionCheckResponse : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static VersionCheckResponse GetRootAsVersionCheckResponse(ByteBuffer _bb) { return GetRootAsVersionCheckResponse(_bb, new VersionCheckResponse()); }
  public static VersionCheckResponse GetRootAsVersionCheckResponse(ByteBuffer _bb, VersionCheckResponse obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public VersionCheckResponse __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Result { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.LoginTable.VersionCheckResponse> CreateVersionCheckResponse(FlatBufferBuilder builder,
      int result = 0) {
    builder.StartTable(1);
    VersionCheckResponse.AddResult(builder, result);
    return VersionCheckResponse.EndVersionCheckResponse(builder);
  }

  public static void StartVersionCheckResponse(FlatBufferBuilder builder) { builder.StartTable(1); }
  public static void AddResult(FlatBufferBuilder builder, int result) { builder.AddInt(0, result, 0); }
  public static Offset<PacketTable.LoginTable.VersionCheckResponse> EndVersionCheckResponse(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.LoginTable.VersionCheckResponse>(o);
  }
}


static public class VersionCheckResponseVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Result*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
