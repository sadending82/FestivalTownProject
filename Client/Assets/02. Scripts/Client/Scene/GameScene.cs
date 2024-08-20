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

        // �ϴ��� �ΰ��ӿ��� �����ϵ��� �����غ���.
        Managers.SetInGameManagers();

        LoadStatue();

        Managers.UI.ShowPopUpUI<UI_Loading>();

        
        //LoadCube();

        //LoadBomb();

        // TODO: ���⼭ ���� ������ ���� �̸� ����� �� �۾� ���� ���־�� �մϴ�.
        // �ϴ��� ������ ��尡 �ϳ��ϱ�, �̰͸� �����ϵ��� �������ָ� �ɵ�?
    }

    public void LoadStatue()
    {
        GameObject Statue1 = Managers.Resource.Instantiate("Statue");
        Statue1.GetComponent<Statue>().SetTeam(0);

        GameObject Statue2 = Managers.Resource.Instantiate("Statue");
        Statue2.transform.position = new Vector3(23.0f, 0.0f, 0.0f);
        Statue2.transform.rotation = Quaternion.Euler(0.0f, -180.0f, 0.0f);
        Statue2.GetComponent<Statue>().SetTeam(1);
    }

    public void LoadCube()
    {
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

    public void LoadBomb()
    {
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

    public override void Clear()
    {
        
    }
}
