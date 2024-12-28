using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_MannequinModel : UI_Base
{
    enum GameObjects
    {
        NonRagdollPlayer,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();

        SetModel();

        transform.LookAt(Camera.main.transform.GetChild(0));
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));


        isInitialized = true;
    }

    public void SetModel()
    {
        var animationState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerAnimationController>();

        animationState.SetLobbyAnimation();

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.localRotation = Quaternion.identity;
    }
    public void EquipResultItem(int itemCode)
    {
        var customizeState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerState>();
        customizeState.ChangeCustomizing(itemCode);
    }
    private void FixedUpdate()
    {
        transform.Rotate(new Vector3(0.0f, Time.deltaTime * 50.0f, 0.0f));
    }
}
