using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_CharacterModel : UI_Base
{

    enum GameObjects
    {
        NonRagdollPlayer
    }

    bool isInitialized = false;

     void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        SetCustomizing();

        isInitialized = true;
    }

    public void SetCustomizing()
    {
        var customizeState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerState>();
        var animationState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerAnimationController>();

        foreach (var customizeData in Managers.Data.PlayerCustomizingData)
        {
            customizeState.ChangeCustomizing(customizeData.Value.ItemCode);
        }

        animationState.SetLobbyAnimation();

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.rotation = Quaternion.identity;
        
        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }



}
