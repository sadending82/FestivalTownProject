using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        // TODO: 여기서 씬이 생성될 때에 미리 해줘야 할 작업 들을 해주어야 합니다.
        // 일단은 지금은 모드가 하나니까, 이것만 관리하도록 설정해주면 될듯?
    }

    public override void Clear()
    {
        
    }
}
