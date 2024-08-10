using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Managers : MonoBehaviour
{
    static Managers s_instance; // static 인스턴스
    static Managers Instance { get { Init(); return s_instance; } }

    UIManager _ui = new UIManager();
    ResourceManager _resource = new ResourceManager();
//    NetworkManager _network;
    PlayerManager _player;
    CubeObjectManager _cubeObject;
    ObjectPoolManager _objPool = new ObjectPoolManager();

    public static UIManager UI { get { return Instance._ui; } }
    public static ResourceManager Resource { get { return Instance._resource; } }
//    public static NetworkManager Network { get { return Instance._network; } }
    public static PlayerManager Player { get { return Instance._player; } }
    public static CubeObjectManager CubeObject { get { return Instance._cubeObject; } }
    public static ObjectPoolManager ObjectPool { get { return Instance._objPool; } }

    private void Start()
    {
        Init();

        // TODO : 나중에는 아래의 매니저 들도 Managers에서 생성하는 것이 바람직해 보입니다.

//        _network = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        _player = GameObject.Find("PlayerManager").GetComponent<PlayerManager>();
        _cubeObject = GameObject.Find("CubeObjectManager").GetComponent<CubeObjectManager>();
    }

    static void Init()
    {
        if (s_instance == null)
        {
            GameObject obj = GameObject.Find("@Managers");
            if (obj == null)
            {
                obj = new GameObject { name = "@Managers" };
                obj.AddComponent<Managers>();
            }

            DontDestroyOnLoad(obj);
            s_instance = obj.GetComponent<Managers>();
        }

        s_instance._objPool.Init();
    }
}
