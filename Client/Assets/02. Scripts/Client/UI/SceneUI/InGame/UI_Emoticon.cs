using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Emoticon : UI_Base
{
    enum GameObjects
    {
        BackgroundImage,
        EmoticonImage,
    }

    float _lifeTime = 5.0f;
    float _curTime = 0.0f;

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    private void Update()
    {
        if (Managers.Game.isInGame)
        {
            _curTime += Time.deltaTime;

            if (_curTime > _lifeTime)
            {
                GameObject.Destroy(gameObject);
            }
        }
    }
}
