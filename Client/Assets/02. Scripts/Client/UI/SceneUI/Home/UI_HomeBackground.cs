using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_HomeBackground : UI_Base
{
    enum GameObjects
    {
        BGBase,
        BGPattern,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        isInitialized = true;
    }

    private void Update()
    {
        
    }
}
