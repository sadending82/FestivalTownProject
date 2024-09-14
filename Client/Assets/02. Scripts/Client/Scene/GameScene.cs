using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEditor.Experimental.GraphView.GraphView;

public class GameScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        Managers.Player.Init();

        LoadStatue();

        LoadCubes();

        LoadBombs();

        LoadWeapons();

        Managers.Network.GetPacketManager().SendGameReady();

        // TODO: 여기서 씬이 생성될 때에 미리 해줘야 할 작업 들을 해주어야 합니다.
        // 일단은 지금은 모드가 하나니까, 이것만 관리하도록 설정해주면 될듯?
    }

    public void LoadStatue()
    {
        GameObject Statue1 = Managers.Resource.Instantiate("Statue");
        Statue1.transform.position = new Vector3(23.0f, 0.0f, 0.0f);
        Statue1.transform.rotation = Quaternion.Euler(0.0f, -90.0f, 0.0f);
        Statue1.GetComponent<Statue>().SetTeam(0);

        GameObject Statue2 = Managers.Resource.Instantiate("Statue");
        Statue2.transform.rotation = Quaternion.Euler(0.0f, 90.0f, 0.0f);
        Statue2.GetComponent<Statue>().SetTeam(1);
    }

    public void LoadCubes()
    {
        Managers.CubeObject.Init();

        List<GameObject> list = new();

        for (int i = 0; i < 30; i++)
        {
            list.Add(Managers.Resource.Instantiate("Cube"));
        }

        foreach(GameObject obj in list)
        {
            Managers.Resource.Destroy(obj);
        }
    }

    public void LoadBombs()
    {
        Managers.BombObject.Init();

        List<GameObject> list = new();

        for (int i = 0; i < 30; i++)
        {
            list.Add(Managers.Resource.Instantiate("Bomb"));
        }

        foreach (GameObject obj in list)
        {
            Managers.Resource.Destroy(obj);
        }
    }

    public void LoadWeapons()
    {
        Managers.WeaponObject.Init();

        List<GameObject> list = new();

        for (int i = 0; i < 10; i++)
        {
            list.Add(Managers.Resource.Instantiate("Frypan"));
            list.Add(Managers.Resource.Instantiate("Bat"));
        }

        foreach (GameObject obj in list)
        {
            Managers.Resource.Destroy(obj);
        }
    }

    public void GameSceneStart()
    {
        Managers.Sound.Play("Sfx_BGM_Ingame", Define.Sound.Bgm);

        Managers.UI.ShowSceneUI<UI_OneVersusOneSceneUI>();

        Managers.Player.SetMyId(Managers.Game.inGameID);
        Managers.Player.SetIsHost(Managers.Game.isHost);
        Managers.Player.SetMyPlayerEnable();

        Managers.Resource.Destroy(GameObject.Find("UI_Loading"));

    }

    public void MoveToResult()
    {
        Managers.UI.CloseAllPopUpUI();
        Managers.UI.CloseSceneUI();

        // TODO: 시상대를 비추기 위한 카메라 이동 및 캐릭터 애니메이션 설정 등이 필요합니다.
        Managers.UI.ShowSceneUI<UI_Result>();

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    public void WaitAndShowResult()
    {
        StartCoroutine(WaitGameEnd());

    }

    IEnumerator WaitGameEnd()
    {
        yield return new WaitUntil(() => Managers.Game.isGameEnd);
        MoveToResult();
    }

    public override void Clear()
    {
        
    }
}
