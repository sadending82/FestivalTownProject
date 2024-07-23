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

public class NetworkManager : MonoBehaviour
{

    public static NetworkManager instance;

    private TcpClient Connection;

    float SendBufferInterval = 2.0f;
    float CurrentTime = 0.0f;

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct TestPacket
    {
        [MarshalAs(UnmanagedType.U2)]
        public ushort size;
        [MarshalAs(UnmanagedType.U2)]
        public ushort type;
    }

    private TestPacket t_packet = new TestPacket();

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

            Debug.Log("Send");
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

    public byte[] CreateTestPacket()
    {
        t_packet.type = 0;
        t_packet.size = (ushort)Marshal.SizeOf(typeof(TestPacket));

        Debug.Log("Make Test Packet.");

        return Serialize<TestPacket>(t_packet);
    }

    private void Update()
    {
        CurrentTime += Time.deltaTime;

        if(CurrentTime > SendBufferInterval)
        {
            CurrentTime -= SendBufferInterval;

            SendPacket(CreateTestPacket());
        }
    }

}
