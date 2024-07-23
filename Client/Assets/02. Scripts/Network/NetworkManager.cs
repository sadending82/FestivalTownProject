using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using System.Runtime.InteropServices;
using PacketTable.Player;
using Google.FlatBuffers;
using NetworkProtocol;

public class NetworkManager : MonoBehaviour
{
    bool isNetworkConnected = false;

    public static NetworkManager instance;

    private TcpClient Connection;

    private PacketManager packetManager;
    private ReceiveManager recvManager;

    float SendBufferInterval = 2.0f;
    float CurrentTime = 0.0f;


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
            return;
        }

        packetManager = new PacketManager();
        recvManager = new ReceiveManager();

    }

    public void Start()
    {
        isNetworkConnected = ConnectToServer();

        if(isNetworkConnected) recvManager.CreateRecvThread(Connection);
    }

    private bool ConnectToServer()
    {
        try
        {
            Connection = new TcpClient("127.0.0.1", 5000);
        }
        catch (Exception e)
        {
            Debug.Log("Error : " + e);
            return false;
        }

        return true;
    }

    private void Update()
    {
        if (isNetworkConnected)
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

                packetManager.SendPacket(Connection, packetManager.CreateTestPacket(packet));
            }
        }
    }

}
