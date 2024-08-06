using System;
using System.Net.Sockets;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    bool isNetworkConnected = false;

    public static NetworkManager instance;

    private TcpClient Connection;

    [SerializeField, Tooltip("Packet Manager")]
    private PacketManager packetManager;

    [SerializeField, Tooltip("Receive Manager")]
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

        packetManager = gameObject.AddComponent<PacketManager>();
        packetManager.Init();
        recvManager = gameObject.AddComponent<ReceiveManager>();
        recvManager.Init(packetManager);

    }

    public void Start()
    {
        isNetworkConnected = ConnectToServer();
        packetManager.SetConnection(Connection);

        Debug.Log("Network Connect : " + isNetworkConnected);

        if(isNetworkConnected) recvManager.CreateRecvThread(Connection);

        recvManager.SetPlayerManager(GameObject.FindWithTag("PlayerManager"));
    }

    private bool ConnectToServer()
    {
        try
        {
            Connection = new TcpClient("39.120.204.67", 45872);
            //Connection = new TcpClient("127.0.0.1", 45872);
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

    public TcpClient GetTcpClient()
    {
        return Connection;
    }
}
