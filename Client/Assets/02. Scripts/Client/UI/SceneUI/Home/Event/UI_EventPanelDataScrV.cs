using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_EventPanelDataScrV : UI_Base
{
    GameObject Content;

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Content = transform.GetChild(0).GetChild(0).gameObject; // scrV -> viewport -> content

        isInitialized = true;
    }

    public GameObject GetContent()
    {
        if (!isInitialized) Init();

        return Content;
    }
}
