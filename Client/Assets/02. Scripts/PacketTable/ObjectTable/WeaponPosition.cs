// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.ObjectTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct WeaponPosition : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static WeaponPosition GetRootAsWeaponPosition(ByteBuffer _bb) { return GetRootAsWeaponPosition(_bb, new WeaponPosition()); }
  public static WeaponPosition GetRootAsWeaponPosition(ByteBuffer _bb, WeaponPosition obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public WeaponPosition __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public PacketTable.UtilitiesTable.Vec3f? Pos { get { int o = __p.__offset(4); return o != 0 ? (PacketTable.UtilitiesTable.Vec3f?)(new PacketTable.UtilitiesTable.Vec3f()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public int Id { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<PacketTable.ObjectTable.WeaponPosition> CreateWeaponPosition(FlatBufferBuilder builder,
      Offset<PacketTable.UtilitiesTable.Vec3f> posOffset = default(Offset<PacketTable.UtilitiesTable.Vec3f>),
      int id = 0) {
    builder.StartTable(2);
    WeaponPosition.AddId(builder, id);
    WeaponPosition.AddPos(builder, posOffset);
    return WeaponPosition.EndWeaponPosition(builder);
  }

  public static void StartWeaponPosition(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddPos(FlatBufferBuilder builder, Offset<PacketTable.UtilitiesTable.Vec3f> posOffset) { builder.AddOffset(0, posOffset.Value, 0); }
  public static void AddId(FlatBufferBuilder builder, int id) { builder.AddInt(1, id, 0); }
  public static Offset<PacketTable.ObjectTable.WeaponPosition> EndWeaponPosition(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.ObjectTable.WeaponPosition>(o);
  }
}


static public class WeaponPositionVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyTable(tablePos, 4 /*Pos*/, PacketTable.UtilitiesTable.Vec3fVerify.Verify, false)
      && verifier.VerifyField(tablePos, 6 /*Id*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
