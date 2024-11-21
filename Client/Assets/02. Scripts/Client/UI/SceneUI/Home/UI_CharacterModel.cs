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
        // 커스터마이징 설정
        // transform도 만지기 때문에, Start에서 해주는 것이 맞는 것으로 보임. 
        SetCustomizing();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));



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

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.localRotation = Quaternion.identity;

        // 홈 화면 용 카메라
        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }

    public void SetInventoryLocalCustomizing()
    {
        var customizeState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerState>();
        var animationState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerAnimationController>();

        foreach (var customizeData in Managers.Data.ClientLocalCustomizingDataDict)
        {
            customizeState.ChangeCustomizing(customizeData.Value.ItemCode);
        }

        animationState.SetLobbyAnimation();

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.localRotation = Quaternion.identity;

        // 인벤토리 용 카메라
        transform.LookAt(Camera.main.transform.GetChild(1).position);
    }

    void Update()
    {
        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }



}
