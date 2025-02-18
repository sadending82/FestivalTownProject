// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace PacketTable.LoginTable
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct SignUpRequest : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static SignUpRequest GetRootAsSignUpRequest(ByteBuffer _bb) { return GetRootAsSignUpRequest(_bb, new SignUpRequest()); }
  public static SignUpRequest GetRootAsSignUpRequest(ByteBuffer _bb, SignUpRequest obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public SignUpRequest __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public string AccountId { get { int o = __p.__offset(4); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetAccountIdBytes() { return __p.__vector_as_span<byte>(4, 1); }
#else
  public ArraySegment<byte>? GetAccountIdBytes() { return __p.__vector_as_arraysegment(4); }
#endif
  public byte[] GetAccountIdArray() { return __p.__vector_as_array<byte>(4); }
  public string AccountPassword { get { int o = __p.__offset(6); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetAccountPasswordBytes() { return __p.__vector_as_span<byte>(6, 1); }
#else
  public ArraySegment<byte>? GetAccountPasswordBytes() { return __p.__vector_as_arraysegment(6); }
#endif
  public byte[] GetAccountPasswordArray() { return __p.__vector_as_array<byte>(6); }
  public string Nickname { get { int o = __p.__offset(8); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetNicknameBytes() { return __p.__vector_as_span<byte>(8, 1); }
#else
  public ArraySegment<byte>? GetNicknameBytes() { return __p.__vector_as_arraysegment(8); }
#endif
  public byte[] GetNicknameArray() { return __p.__vector_as_array<byte>(8); }

  public static Offset<PacketTable.LoginTable.SignUpRequest> CreateSignUpRequest(FlatBufferBuilder builder,
      StringOffset account_idOffset = default(StringOffset),
      StringOffset account_passwordOffset = default(StringOffset),
      StringOffset nicknameOffset = default(StringOffset)) {
    builder.StartTable(3);
    SignUpRequest.AddNickname(builder, nicknameOffset);
    SignUpRequest.AddAccountPassword(builder, account_passwordOffset);
    SignUpRequest.AddAccountId(builder, account_idOffset);
    return SignUpRequest.EndSignUpRequest(builder);
  }

  public static void StartSignUpRequest(FlatBufferBuilder builder) { builder.StartTable(3); }
  public static void AddAccountId(FlatBufferBuilder builder, StringOffset accountIdOffset) { builder.AddOffset(0, accountIdOffset.Value, 0); }
  public static void AddAccountPassword(FlatBufferBuilder builder, StringOffset accountPasswordOffset) { builder.AddOffset(1, accountPasswordOffset.Value, 0); }
  public static void AddNickname(FlatBufferBuilder builder, StringOffset nicknameOffset) { builder.AddOffset(2, nicknameOffset.Value, 0); }
  public static Offset<PacketTable.LoginTable.SignUpRequest> EndSignUpRequest(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<PacketTable.LoginTable.SignUpRequest>(o);
  }
}


static public class SignUpRequestVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyString(tablePos, 4 /*AccountId*/, false)
      && verifier.VerifyString(tablePos, 6 /*AccountPassword*/, false)
      && verifier.VerifyString(tablePos, 8 /*Nickname*/, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
