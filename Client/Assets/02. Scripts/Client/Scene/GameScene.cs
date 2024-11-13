using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using NetworkProtocol;

public class GameScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        Managers.Game.LoadGameMap();

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
        switch (Managers.Game.mapCode)
        {
            case MapCode.Map_FITH_1vs1:
                {
                    GameObject Statue1 = Managers.Resource.Instantiate("Statue");
                    Statue1.transform.position = new Vector3(-3.0f, 0.0f, 10.0f);
                    Statue1.transform.rotation = Quaternion.Euler(0.0f, 90.0f, 0.0f);
                    Statue1.GetComponent<Statue>().SetTeam(0);

                    GameObject Statue2 = Managers.Resource.Instantiate("Statue");
                    Statue2.transform.position = new Vector3(43.0f, 0.0f, 10.0f);
                    Statue2.transform.rotation = Quaternion.Euler(0.0f, -90.0f, 0.0f);
                    Statue2.GetComponent<Statue>().SetTeam(1);
                }
                break;

            case MapCode.Map_FITH_1vs1vs1:
                {
                    GameObject Statue1 = Managers.Resource.Instantiate("Statue");
                    Statue1.transform.position = new Vector3(22.0f, 0.0f, 31.0f);
                    Statue1.transform.rotation = Quaternion.Euler(0.0f, 180.0f, 0.0f);
                    Statue1.GetComponent<Statue>().SetTeam(0);

                    GameObject Statue2 = Managers.Resource.Instantiate("Statue");
                    Statue2.transform.position = new Vector3(-3.0f, 0.0f, 6.0f);
                    Statue2.transform.rotation = Quaternion.Euler(0.0f, 90.0f, 0.0f);
                    Statue2.GetComponent<Statue>().SetTeam(1);

                    GameObject Statue3 = Managers.Resource.Instantiate("Statue");
                    Statue3.transform.position = new Vector3(47.0f, 0.0f, 6.0f);
                    Statue3.transform.rotation = Quaternion.Euler(0.0f, -90.0f, 0.0f);
                    Statue3.GetComponent<Statue>().SetTeam(2);
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! LoadStatue(): Wrong MapCode!!!");
                }
                break;
        }
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

        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;

        Managers.Player.SetMyId(Managers.Game.inGameID);
        Managers.Player.SetIsHost(Managers.Game.isHost);
        Managers.Player.SetMyPlayerEnable();

        Managers.Resource.Destroy(GameObject.Find("UI_Loading"));

        Managers.PostProcess.SetPostProcesser();

        StartCoroutine(GameStartTimerActive());
    }

    IEnumerator GameStartTimerActive()
    {
        var ui = Managers.UI.ShowPopUpUI<UI_StartTimer>();
        Managers.Sound.Play("Sfx_Countdown");
        yield return null;
        ui.TimerSet(3);
        yield return new WaitForSeconds(1f);
        ui.TimerSet(2);
        yield return new WaitForSeconds(1f);
        ui.TimerSet(1);
        yield return new WaitUntil(() => Managers.Game.isTimerStart);
        ui.TimerSet(0);
        yield return new WaitForSeconds(0.5f);
        Managers.UI.ClosePopUpUI(ui);
    }

    public void MoveToResult()
    {
        Managers.UI.CloseAllPopUpUI();
        Managers.UI.CloseSceneUI();

        // TODO: 시상대를 비추기 위한 카메라 이동 및 캐릭터 애니메이션 설정 등이 필요합니다.
        int myId = Managers.Player.GetMyId();
        GameObject tPlayer = Managers.Player.FindPlayerById(myId);
        tPlayer.GetComponent<CharacterStatus>().CameraOff();
        ResultObjectOn();

        Managers.Sound.Stop(Define.Sound.Bgm);

        if(Managers.Game.GetWinningTeam() ==
            tPlayer.GetComponent<CharacterStatus>().GetTeamNumber())
        {
            Managers.Sound.Play("Sfx_Win");
        }
        else
        {
            Managers.Sound.Play("Sfx_Lose");
        }

        var ui = Managers.UI.ShowSceneUI<UI_Result>();
        ui.Init();

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
        ResultObjectOff();
    }

    private void ResultObjectOn()
    {
        GameObject resultScene = GameObject.Find("ResultScene");
        for(int i = 0;i< resultScene.transform.childCount;++i)
        {
            resultScene.transform.GetChild(i).gameObject.SetActive(true);
        }
        ResultPlayerController resultPlayerController = resultScene.transform.GetChild(0).GetComponent<ResultPlayerController>();
        resultPlayerController.SetPlayer();
    }
    private void ResultObjectOff()
    {
        GameObject resultScene = GameObject.Find("ResultScene");
        ResultPlayerController resultPlayerController = resultScene.transform.GetChild(0).GetComponent<ResultPlayerController>();
        resultPlayerController.ResetPlayer();
        for (int i = 0; i < resultScene.transform.childCount; ++i)
        {
            resultScene.transform.GetChild(i).gameObject.SetActive(false);
        }
    }
}
