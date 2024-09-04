using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Managers : MonoBehaviour
{
    static Managers s_instance; // static 인스턴스
    static Managers Instance { get { Init(); return s_instance; } }

    UIManager _ui = new UIManager();
    ResourceManager _resource = new ResourceManager();
    NetworkManager _network;
    PlayerManager _player;
    ObjectPoolManager _objPool = new ObjectPoolManager();
    DataManager _data = new DataManager();
    SceneManagerEx _scene = new SceneManagerEx();
    GameManager _game;

    CubeObjectManager _cubeObject = new CubeObjectManager();
    BombObjectManager _bombObject = new BombObjectManager();
    WeaponObjectManager _weaponObject = new WeaponObjectManager();

    public static UIManager UI { get { return Instance._ui; } }
    public static ResourceManager Resource { get { return Instance._resource; } }
    public static NetworkManager Network { get { return Instance._network; } }
    public static PlayerManager Player { get { return Instance._player; } }
    public static CubeObjectManager CubeObject { get { return Instance._cubeObject; } }
    public static BombObjectManager BombObject { get { return Instance._bombObject; } }
    public static ObjectPoolManager ObjectPool { get { return Instance._objPool; } }

    public static DataManager Data { get { return Instance._data; } }

    public static SceneManagerEx Scene { get { return Instance._scene; } }

    public static GameManager Game { get { return Instance._game; } }

    public static WeaponObjectManager WeaponObject { get { return Instance._weaponObject; } }

    private void Start()
    {
        _game = gameObject.AddComponent<GameManager>();
        _weaponObject = gameObject.AddComponent<WeaponObjectManager>();
        _cubeObject = gameObject.AddComponent<CubeObjectManager>();
        _bombObject = gameObject.AddComponent<BombObjectManager>();

        Init();
        _game.Init();

        // TODO : 나중에는 아래의 매니저 들도 Managers에서 생성하는 것이 바람직해 보입니다.
        _network = gameObject.AddComponent<NetworkManager>();
    }

    public static void SetInGameManagers()
    {
        // 이런 문제 때문에 기본적으로 Managers에서 생성하는 것이 바람직하다고 한 것이긴 합니다.
        // 게임 내부에서만 필요한 매니저들의 경우 Scene에서 생성해서 관리해야 할 텐데...
        s_instance._player = GameObject.Find("PlayerManager").GetComponent<PlayerManager>();
    }

    public static void DeleteInGameManagers()
    {
        s_instance._player = null;
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

            s_instance._objPool.Init();
            s_instance._data.Init();
            //s_instance._game.Init();
        }
    }

    static public void Clear()
    {
        // TODO: 씬이 교체될 때 비워줘야 할 것들 처리해 주어야 합니다.
        // 대표적으로 UI 같은게 있겠네요

        Scene.Clear();
        UI.Clear();
        DeleteInGameManagers();
    }
}
