// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.ShopTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PurchaseGoodsRequest : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PurchaseGoodsRequest GetRootAsPurchaseGoodsRequest(ByteBuffer _bb) { return GetRootAsPurchaseGoodsRequest(_bb, new PurchaseGoodsRequest()); }
  public static PurchaseGoodsRequest GetRootAsPurchaseGoodsRequest(ByteBuffer _bb, PurchaseGoodsRequest obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PurchaseGoodsRequest __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Index { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.ShopTable.PurchaseGoodsRequest> CreatePurchaseGoodsRequest(FlatBufferBuilder builder,
      int index = 0) {
    builder.StartTable(1);
    PurchaseGoodsRequest.AddIndex(builder, index);
    return PurchaseGoodsRequest.EndPurchaseGoodsRequest(builder);
  }

  public static void StartPurchaseGoodsRequest(FlatBufferBuilder builder) { builder.StartTable(1); }
  public static void AddIndex(FlatBufferBuilder builder, int index) { builder.AddInt(0, index, 0); }
  public static Offset<PacketTable.ShopTable.PurchaseGoodsRequest> EndPurchaseGoodsRequest(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.ShopTable.PurchaseGoodsRequest>(o);
  }
}


static public class PurchaseGoodsRequestVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Index*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
