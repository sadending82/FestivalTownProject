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

        // TODO: 여기서 씬이 생성될 때에 미리 해줘야 할 작업 들을 해주어야 합니다.
        // 일단은 지금은 모드가 하나니까, 이것만 관리하도록 설정해주면 될듯?
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
