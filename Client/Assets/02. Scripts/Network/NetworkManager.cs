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
        recvManager = new ReceiveManager(packetManager);

    }

    public void Start()
    {
        isNetworkConnected = ConnectToServer();

        Debug.Log("Network Connect : " + isNetworkConnected);

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

    public PacketManager GetPacketManager()
    {
        return packetManager;
    }

    public ReceiveManager GetReceiveManager()
    {
        return recvManager;
    }

    private void Update()
    {
        if (isNetworkConnected)
        {
            CurrentTime += Time.deltaTime;

            if (CurrentTime > SendBufferInterval)
            {
                CurrentTime -= SendBufferInterval;
            }
        }
    }

}
