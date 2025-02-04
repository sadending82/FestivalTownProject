using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Managers : MonoBehaviour
{
    static Managers s_instance; // static 인스턴스
    static Managers Instance { get { Init(); return s_instance; } }

    //------------
    UIManager _ui = new UIManager();
    ResourceManager _resource = new ResourceManager();
    NetworkManager _network;
    DataManager _data = new DataManager();
    SceneManagerEx _scene = new SceneManagerEx();
    SoundManager _sound = new SoundManager();
    CursorManager _cursor;
    //------------

    ObjectPoolManager _objPool = new ObjectPoolManager();
    GameManager _game;
    PlayerManager _player;
    CubeObjectManager _cubeObject;
    BombObjectManager _bombObject;
    WeaponObjectManager _weaponObject;
    SpectatorCameraManager _spectatorCamera;
    PostProcessManager _postProcess = new PostProcessManager();
    EffectManager _effect;
    MapManager _map;

    //------------

    CheatManager _cheat = new CheatManager();

    public static UIManager UI { get { return Instance._ui; } }
    public static ResourceManager Resource { get { return Instance._resource; } }
    public static NetworkManager Network { get { return Instance._network; } }
    public static DataManager Data { get { return Instance._data; } }
    public static SceneManagerEx Scene { get { return Instance._scene; } }

    public static SoundManager Sound { get { return Instance._sound; } }

    public static CursorManager Cursor { get { return Instance._cursor; } }

    //------------
    public static ObjectPoolManager ObjectPool { get { return Instance._objPool; } }
    public static GameManager Game { get { return Instance._game; } }
    public static PlayerManager Player { get { return Instance._player; } }
    public static CubeObjectManager CubeObject { get { return Instance._cubeObject; } }
    public static BombObjectManager BombObject { get { return Instance._bombObject; } }
    public static WeaponObjectManager WeaponObject { get { return Instance._weaponObject; } }
    public static SpectatorCameraManager SpectatorCamera { get { return Instance._spectatorCamera; } }
    public static PostProcessManager PostProcess { get { return Instance._postProcess; } }
    public static EffectManager Effect { get { return Instance._effect; } }
    public static MapManager Map { get { return Instance._map; } }

    //-----------

    public static CheatManager Cheat { get { return Instance._cheat; } }

    private void Awake()
    {
        Debug.unityLogger.logEnabled = Debug.isDebugBuild;
    }

    private void Start()
    {
        

        _cursor = gameObject.AddComponent<CursorManager>();
        _game = gameObject.AddComponent<GameManager>();
        _weaponObject = gameObject.AddComponent<WeaponObjectManager>();
        _cubeObject = gameObject.AddComponent<CubeObjectManager>();
        _bombObject = gameObject.AddComponent<BombObjectManager>();
        _player = gameObject.AddComponent<PlayerManager>();
        _spectatorCamera = gameObject.AddComponent<SpectatorCameraManager>();
        _effect = gameObject.AddComponent<EffectManager>();
        _map = gameObject.AddComponent<MapManager>();

        Init();
        _game.Init();

        _network = gameObject.AddComponent<NetworkManager>();
        StartCoroutine(InitSoundSetting());
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
            s_instance._sound.Init();
            s_instance._cheat.Init();
            //s_instance._game.Init();
        }
    }

    IEnumerator InitSoundSetting()
    {
        yield return null;
        s_instance._sound.InitSoundSetting();
        yield break;
    }

    static public void Clear()
    {
        // TODO: 씬이 교체될 때 비워줘야 할 것들 처리해 주어야 합니다.
        // 대표적으로 UI 같은게 있겠네요
        Sound.Clear();
        Scene.Clear();
        UI.Clear();
        PostProcess.Clear();
    }
}
