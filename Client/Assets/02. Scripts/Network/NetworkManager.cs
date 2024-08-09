using System;
using System.Net.Sockets;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;

public class NetworkSelect : EditorWindow
{
    static NetworkManager _nManager;

    int _networkSelected;
    string _ipAddressText;

    [MenuItem("Network/NetworkSelect %#F5")]

    static void Open()
    {
        GetWindow<NetworkSelect>();
        _nManager = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
    }

    private void OnGUI()
    {
        if (_nManager == null)
        {
            _nManager = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        }

        GUIStyle labelStyle = EditorStyles.boldLabel;
        labelStyle.alignment = TextAnchor.MiddleCenter;
        GUILayout.Label("네트워크 설정", EditorStyles.boldLabel);
        

        _networkSelected = GUILayout.Toolbar(_networkSelected, new string[] { "로컬 네트워크로 설정", "리모트 네트워크로 설정", "IP 직접 입력" });

        if (_networkSelected == 2)
        {
            GUILayout.Space(10);
            GUILayout.Label("IP 주소");
            _ipAddressText = GUILayout.TextField(_ipAddressText);

        }

        switch(_networkSelected)
        {
            case 0:
                _nManager._ipAddress = "127.0.0.1";
                break;
            case 1:
                _nManager._ipAddress = "39.120.204.67";
                break;
            case 2:
                _nManager._ipAddress = _ipAddressText;
                break;
            default:
                break;
        }

        labelStyle.alignment = TextAnchor.MiddleLeft;

        minSize = new Vector3(480.0f, 200.0f);
        maxSize = new Vector3(480.0f, 200.0f);
    }
}
#endif

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
        isNetworkConnected = ConnectToServer();
        packetManager.SetConnection(Connection);

        Debug.Log("Network Connect : " + isNetworkConnected);

        if(isNetworkConnected) recvManager.CreateRecvThread(Connection);
    }

    private bool ConnectToServer()
    {
        try
        {

            if (_ipAddress != "")
            {
                Connection = new TcpClient(_ipAddress, 45872);
            }
            else throw new Exception("ip 설정 안 되었어요...");

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
