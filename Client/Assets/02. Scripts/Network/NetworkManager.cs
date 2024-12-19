using System;
using System.Collections;
using System.Net.Sockets;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    bool isNetworkConnected = false;
    bool isConnectionLostNoticed = false;

    public static NetworkManager instance;

    private TcpClient Connection;

    [SerializeField, Tooltip("Packet Manager")]
    private PacketManager packetManager;

    [SerializeField, Tooltip("Receive Manager")]
    private ReceiveManager recvManager;

    [SerializeField, Tooltip("IPAdrees")]
    private IPAddressData IPAddressObj;

    [SerializeField, Tooltip("PortNumber")]
    private PortNumberData PortObj;

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
        PortObj = Resources.Load<PortNumberData>("ScriptableObject/PortNumber");

        isNetworkConnected = ConnectToServer();
        packetManager.SetConnection(Connection);

        Debug.Log("Network Connect : " + isNetworkConnected);

        if (isNetworkConnected)
        {
            recvManager.CreateRecvThread(Connection);
            // 접속 성공 시 버전 체크
            packetManager.SendVersionCheckRequestPacket();
        }
        else
        {
            StartCoroutine(ShowNoticePopUp());
        }
    }

    private bool ConnectToServer()
    {
        try
        {
#if UNITY_EDITOR
            if (IPAddressObj.IPAddress != "")
            {
                Connection = new TcpClient(IPAddressObj.IPAddress, int.Parse(PortObj.PortNumber));
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

            if (!Connection.Connected)
            {
                if (!isConnectionLostNoticed)
                {
                    isConnectionLostNoticed = true;

                    //UI가 바인드 되기 전에는 내부 이름을 수정할 수 없으므로 코루틴으로 한 프레임 뒤에 변경하도록 해줌
                    StartCoroutine(ShowNoticePopUp());
                }
            }
        }
    }

    IEnumerator ShowNoticePopUp()
    {
        var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
        yield return null;
        ui.NoticeTextChange("서버에 접속할 수 없습니다.");
        ui.BindGameEndEvent();
    }

    public TcpClient GetTcpClient()
    {
        return Connection;
    }
}
