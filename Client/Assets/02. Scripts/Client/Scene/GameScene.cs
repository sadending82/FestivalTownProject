using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class GameScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        LoadStatue();

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

    public override void Clear()
    {
        
    }
}
