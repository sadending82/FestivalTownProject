using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using NetworkProtocol;
using System.Reflection.Emit;

public class GameScene : BaseScene
{
    bool isLoadStart = false;
    float LoadTime = 7.0f;

    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        Managers.Map.Init();

        Managers.Game.LoadGameMap();

        Managers.Player.Init();

        // ������ Init �ܰ迡�� ���� ��Ŷ �����ִ� ��, �ڷ�ƾ���� �ذ��ϰ� �ֽ��ϴ�.
        // �̴� ����ũ �ε��� �ֱ� ���ؼ� �Դϴ�.

    }

    private void Start()
    {
        if (!isLoadStart)
        {
            StartCoroutine(LoadGameScene());
            isLoadStart = true;
        }
    }

    public void LoadPlayers()
    {
        foreach (var pData in Managers.Game.GetPlayerDataDict())
        {
            
            Managers.Game.PlayerTeamData.Add(pData.Value.id, pData.Value.team);
            Managers.Player.GetComponent<PlayerManager>().AddPlayer(pData.Value.id, pData.Value.pos, pData.Value.team, pData.Value.nickname);
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

        var ui = Managers.UI.ShowSceneUI<UI_IngameScene>();
        ui.Init();
        ui.SetCurrentClientTeam(Managers.Game.currentTeamID);
        ui.PointPanelPositionSet();

        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;

        Managers.Player.SetMyId(Managers.Game.inGameID);
        Managers.Player.SetIsHost(Managers.Game.isHost);
        Managers.Player.SetMyPlayerEnable();

        Managers.Resource.Destroy(GameObject.Find("UI_Loading"));

        Managers.PostProcess.SetPostProcesser();

        Managers.Game.isInGame = true;

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

        // TODO: �û�븦 ���߱� ���� ī�޶� �̵� �� ĳ���� �ִϸ��̼� ���� ���� �ʿ��մϴ�.
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

        Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
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

    IEnumerator LoadGameScene()
    {
        float curtime = 0.0f;
        var loadUi = GameObject.Find("UI_Loading").GetComponent<UI_Loading>();

        LoadPlayers();

        LoadCubes();
        yield return null;
        curtime += Time.deltaTime;
        loadUi.SetProgressBarPersentage(curtime / LoadTime);

        LoadBombs();
        yield return null;
        curtime += Time.deltaTime;
        loadUi.SetProgressBarPersentage(curtime / LoadTime);

        LoadWeapons();
        yield return null;
        curtime += Time.deltaTime;
        loadUi.SetProgressBarPersentage(curtime / LoadTime);

        yield return null;

        while (curtime < LoadTime)
        {
            curtime += Time.deltaTime;
            loadUi.SetProgressBarPersentage(curtime / LoadTime);
            yield return null;
            
        }

        Managers.Network.GetPacketManager().SendGameReady();
        Debug.Log("Send Game Ready Packet");
        // TODO: ���⼭ ���� ������ ���� �̸� ����� �� �۾� ���� ���־�� �մϴ�.
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
        NonRagdollPlayerController resultPlayerController = resultScene.transform.GetChild(0).GetComponent<NonRagdollPlayerController>();
        resultPlayerController.SetPlayer();
    }
    private void ResultObjectOff()
    {
        GameObject resultScene = GameObject.Find("ResultScene");
        NonRagdollPlayerController resultPlayerController = resultScene.transform.GetChild(0).GetComponent<NonRagdollPlayerController>();
        resultPlayerController.ResetPlayer();
        for (int i = 0; i < resultScene.transform.childCount; ++i)
        {
            resultScene.transform.GetChild(i).gameObject.SetActive(false);
        }
    }

    private void Update()
    {
#if UNITY_EDITOR
#elif UNITY_STANDALONE_WIN
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (Managers.Game.isTimerStart)
            {
                var tui = Managers.UI.GetTopOfPopUPUI();
                UI_IngameEscapeButton iebUi = null;
                if (tui != null) { iebUi = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_IngameEscapeButton>(); }

                if (tui == null || iebUi == null)
                {
                    var ui = Managers.UI.ShowPopUpUI<UI_IngameEscapeButton>();
                    ui.Init();
                    ui.NoticeTextChange("���� ���� ������ Esc ������ �����°�\nȮ�� ������ �� ����");

                    Cursor.lockState = CursorLockMode.None;
                    Cursor.visible = true;
                }

            }
        }
#endif
    }


}
