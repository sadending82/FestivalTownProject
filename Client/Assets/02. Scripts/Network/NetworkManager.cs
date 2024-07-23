using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.UIElements;
using System.Runtime.InteropServices;
using PacketTable.Player;
using Google.FlatBuffers;

public class NetworkManager : MonoBehaviour
{

    public static NetworkManager instance;

    private TcpClient Connection;

    float SendBufferInterval = 2.0f;
    float CurrentTime = 0.0f;

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct HEADER
    {
        [MarshalAs(UnmanagedType.U2)]
        public ushort size;
        [MarshalAs(UnmanagedType.U2)]
        public ushort type;
    }


    void Awake()
    {
        DontDestroyOnLoad(gameObject);

        if(instance == null)
        {
            instance = this;
        }
        else if (instance != this)
        {
            Destroy(gameObject);
        }

    }

    public void Start()
    {
        ConnectToServer();
    }

    private void ConnectToServer()
    {
        try
        {
            Connection = new TcpClient("127.0.0.1", 5000);
        }
        catch (Exception e)
        {
            Debug.Log("Error : " + e);
        }
    }

    private void SendPacket(Byte[] buffer)
    {
        if (Connection == null)
        {
            return;
        }
        try
        {
            NetworkStream stream = Connection.GetStream();
            if(stream.CanWrite)
            {
                stream.Write(buffer, 0, buffer.Length);
            }

            Debug.Log("Send: " + buffer.Length);
        }
        catch(SocketException Exception)
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
        CurrentTime += Time.deltaTime;

        if (CurrentTime > SendBufferInterval)
        {
            CurrentTime -= SendBufferInterval;

            var builder = new FlatBufferBuilder(1);

            var pos = Vec3.CreateVec3(builder, 1.0f, 2.0f, 3.0f);

            PlayerMove.StartPlayerMove(builder);
            PlayerMove.AddKey(builder, 1);
            PlayerMove.AddPos(builder, pos);
            PlayerMove.AddDirection(builder, pos);
            var MoveData = PlayerMove.EndPlayerMove(builder);
            builder.Finish(MoveData.Value);
            var packet = builder.SizedByteArray();

            SendPacket(CreateTestPacket(packet));
        }
    }

}
