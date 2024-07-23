using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace ActiveRagdoll
{
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
        private AnimationModule animationMoudule;

        private void Awake()
        {
        }
        private void Start()
        {
            // 기본 스탯(ex.speed, stamina..)에 따라 세부 스탯(ex. rollSpeed, runSpeed..)들 설정
            animationMoudule = this.GetComponent<AnimationModule>();
            SetUseAnimationModule(true);
            SetUseIK(false);
        }
        private void Update()
        {
        }

        // ------------------- GETTERS & SETTERS -------------------
        private void SetUseIK(bool useIK)
        {
            this.useIK = useIK;
            animationMoudule.SetUseIK(useIK);
        }
        private void SetUseAnimationModule(bool useAnimationModule)
        {
            this.useAnimationModule = useAnimationModule;
            animationMoudule.SetUseAnimationModule(useAnimationModule);
        }
    }
}