using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEditor.MemoryProfiler;
using UnityEngine;
using NetworkProtocol;

public class PacketManager
{

    public void SendPacket(TcpClient Connection, Byte[] buffer)
    {
        if (Connection == null)
        {
            return;
        }
        try
        {
            NetworkStream stream = Connection.GetStream();
            if (stream.CanWrite)
            {
                stream.Write(buffer, 0, buffer.Length);
            }

            Debug.Log("Send");
        }
        catch (SocketException Exception)
        {
            Debug.Log("Write exception: " + Exception);
        }
    }

    private byte[] Serialize<T>(T packet)
    {
        var buffer = new byte[Marshal.SizeOf(typeof(T))];

        var gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        var pBuffer = gch.AddrOfPinnedObject();

        Marshal.StructureToPtr(packet, pBuffer, false);
        gch.Free();

        Debug.Log("Serializing.");

        return buffer;
    }

    public byte[] CreatePlayerMovePacket(byte[] data, Vector3Int pos, Vector3Int direction)
    {
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERMOVE, size = (ushort)data.Length };
        
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] buf = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, buf, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, buf, headerdata.Length, data.Length);

        Debug.Log("Make Move Packet.");

        return buf;
    }

    public byte[] CreatePlayerStopPacket(byte[] data, Vector3Int pos, Vector3Int direction)
    {
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERSTOP, size = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] buf = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, buf, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, buf, headerdata.Length, data.Length);

        Debug.Log("Make Move Packet.");

        return buf;
    }

    private void Update()
    {

    }
}
