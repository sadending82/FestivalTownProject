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
    private ActiveRagdoll.AnimationModule animationMoudule;

    private void Awake()
    {
    }
    private void Start()
    {
    }
    private void Update()
    {
    }

    // ------------------- GETTERS & SETTERS -------------------
}