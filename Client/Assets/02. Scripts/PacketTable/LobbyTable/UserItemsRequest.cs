// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.LobbyTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct UserItemsRequest : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static UserItemsRequest GetRootAsUserItemsRequest(ByteBuffer _bb) { return GetRootAsUserItemsRequest(_bb, new UserItemsRequest()); }
  public static UserItemsRequest GetRootAsUserItemsRequest(ByteBuffer _bb, UserItemsRequest obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public UserItemsRequest __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }


  public static void StartUserItemsRequest(FlatBufferBuilder builder) { builder.StartTable(0); }
  public static Offset<PacketTable.LobbyTable.UserItemsRequest> EndUserItemsRequest(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.LobbyTable.UserItemsRequest>(o);
  }
}


static public class UserItemsRequestVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
