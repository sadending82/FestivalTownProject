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

        LoadStatue();

        Managers.Sound.Play("Sfx_BGM_Ingame", Define.Sound.Bgm);

        Managers.UI.ShowSceneUI<UI_OneVersusOneSceneUI>();


        Managers.Player.Init();

        Managers.Player.SetMyId(Managers.Game.inGameID);
        Managers.Player.SetIsHost(Managers.Game.isHost);
        Managers.Player.SetMyPlayerEnable();

        LoadCubes();

        LoadBombs();

        LoadWeapons();

        // TODO: ���⼭ ���� ������ ���� �̸� ����� �� �۾� ���� ���־�� �մϴ�.
        // �ϴ��� ������ ��尡 �ϳ��ϱ�, �̰͸� �����ϵ��� �������ָ� �ɵ�?
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

    public override void Clear()
    {
        
    }
}
