using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GachaponModel : UI_Base
{
    enum GameObjects
    {
        Gachpons,
        RaycastCatcher
    }

    bool isInitialized = false;
    bool _isClicked = false;
    void Start()
    {
        if (!isInitialized) Init();

        SetGachapon();

        if (Camera.main.transform.GetChild(2).gameObject.activeInHierarchy)
        {
            transform.LookAt(Camera.main.transform.GetChild(2));
        }
    }
    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }
    public void SetGachapon()
    {
        Get<GameObject>((int)GameObjects.Gachpons).transform.localRotation = Quaternion.identity;

        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }
}
