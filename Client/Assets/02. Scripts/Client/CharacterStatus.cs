using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterStatus : MonoBehaviour
{
    [Header("--- Status ---")]
    public int maxHp;
    public int hp;
    public float maxStamina;
    public float stamina;
    public float staminaRecoverySpeed;
    public int strength;
    public int speed;
    public float runSpeedRatio;
    private float rollSpeed;
    public bool isDie = false;
    public int groggyNum;
    public float groggyTime;

    [Header("--- AnimationControll ---")]
    public bool isGroggy;
    private bool useIK;
    private bool useAnimationModule;
    private ActiveRagdoll.AnimationModule animationMoudule;

    private void Awake()
    {
    }
    private void Start()
    {
        // �⺻ ����(ex.speed, stamina..)�� ���� ���� ����(ex. rollSpeed, runSpeed..)�� ����
        animationMoudule = this.GetComponent<ActiveRagdoll.AnimationModule>();
        SetUseAnimationModule(true);
        SetUseIK(false);
    }
    private void Update()
    {
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void SetUseIK(bool useIK)
    {
        this.useIK = useIK;
        animationMoudule.SetUseIK(useIK);
    }
    public void SetUseAnimationModule(bool useAnimationModule)
    {
        this.useAnimationModule = useAnimationModule;
        animationMoudule.SetUseAnimationModule(useAnimationModule);
    }
}