using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEditor.MemoryProfiler;
using UnityEngine;

public class PacketManager : MonoBehaviour
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct HEADER
    {
        [MarshalAs(UnmanagedType.U2)]
        public ushort size;
        [MarshalAs(UnmanagedType.U2)]
        public ushort type;
    }

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
            Debug.Log("Socket exception: " + Exception);
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

    public byte[] CreateTestPacket(byte[] data)
    {
        HEADER header = new HEADER { type = 3, size = (ushort)data.Length };
        
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] buf = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, buf, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, buf, headerdata.Length, data.Length);

        Debug.Log("Make Test Packet.");

        return buf;
    }

    private void Update()
    {

    }
}
