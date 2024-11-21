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
        // Ŀ���͸���¡ ����
        // transform�� ������ ������, Start���� ���ִ� ���� �´� ������ ����. 
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

        // Ȩ ȭ�� �� ī�޶�
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

        // �κ��丮 �� ī�޶�
        transform.LookAt(Camera.main.transform.GetChild(1).position);
    }

    void Update()
    {
        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }



}
