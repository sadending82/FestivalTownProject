// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.ObjectTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct RootObject : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static RootObject GetRootAsRootObject(ByteBuffer _bb) { return GetRootAsRootObject(_bb, new RootObject()); }
  public static RootObject GetRootAsRootObject(ByteBuffer _bb, RootObject obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyRootObject(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, RootObjectVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public RootObject __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public PacketTable.ObjectTable.Vec3f? Pos { get { int o = __p.__offset(4); return o != 0 ? (PacketTable.ObjectTable.Vec3f?)(new PacketTable.ObjectTable.Vec3f()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public int Id { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.ObjectTable.RootObject> CreateRootObject(FlatBufferBuilder builder,
      Offset<PacketTable.ObjectTable.Vec3f> posOffset = default(Offset<PacketTable.ObjectTable.Vec3f>),
      int id = 0) {
    builder.StartTable(2);
    RootObject.AddId(builder, id);
    RootObject.AddPos(builder, posOffset);
    return RootObject.EndRootObject(builder);
  }

  public static void StartRootObject(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddPos(FlatBufferBuilder builder, Offset<PacketTable.ObjectTable.Vec3f> posOffset) { builder.AddOffset(0, posOffset.Value, 0); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(1, id, 0); }
  public static Offset<PacketTable.ObjectTable.RootObject> EndRootObject(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.ObjectTable.RootObject>(o);
  }
  public static void FinishRootObjectBuffer(FlatBufferBuilder builder, Offset<PacketTable.ObjectTable.RootObject> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedRootObjectBuffer(FlatBufferBuilder builder, Offset<PacketTable.ObjectTable.RootObject> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class RootObjectVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyTable(tablePos, 4 /*Pos*/, PacketTable.ObjectTable.Vec3fVerify.Verify, false)
      && verifier.VerifyField(tablePos, 6 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}