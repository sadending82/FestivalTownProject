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

    [SerializeField, Tooltip("IPAdrees")]
    private IPAddressData IPAddressObj;

    float SendBufferInterval = 2.0f;
    float CurrentTime = 0.0f;

    [HideInInspector]
    public string _ipAddress = "";

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
        IPAddressObj = Resources.Load<IPAddressData>("ScriptableObject/IPAddress");

        isNetworkConnected = ConnectToServer();
        packetManager.SetConnection(Connection);

        Debug.Log("Network Connect : " + isNetworkConnected);

        if(isNetworkConnected) recvManager.CreateRecvThread(Connection);
    }

    private bool ConnectToServer()
    {
        try
        {
#if UNITY_EDITOR
            if (IPAddressObj.IPAddress != "")
            {
                Connection = new TcpClient(IPAddressObj.IPAddress, 45872);
            }
            else throw new Exception("ip 설정 안 되었어요...");
#elif UNITY_STANDALONE_WIN

            if (IPAddressObj.IPAddress != "")
            {
                Connection = new TcpClient(IPAddressObj.IPAddress, 45872);
            }
            else throw new Exception("ip 설정 안 되었어요...");
#endif
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
