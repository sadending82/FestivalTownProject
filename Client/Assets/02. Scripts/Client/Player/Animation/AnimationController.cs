using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class AnimationController : MonoBehaviour
{
    // 애니메이션은 상체는 서버와 상태를 교환하고 하체는 클라에서 관리한다.
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    public ActiveRagdoll.AnimationModule animationModule;

    [Header("--- Animation ---")]
    public Animator animator;

    [Header("--- IK ---")]
    // 타겟
    public Transform targetLeftHand;
    public Transform targetRightHand;
    public Transform targetLeftHint;
    public Transform targetRightHint;
    // 일반 공격 관련
    public Transform attackLeftHand;
    public Transform attackRightHand;
    public Transform attackLeftHint;
    public Transform attackRightHint;

    public CharacterStatus playerStatus;
    private float attackSpeed;
    private float attackTime;
    private bool isLeftAttack;

    private void Awake()
    {   
        upperBodyAnimationState = UpperBodyAnimationState.NONE;
        lowerBodyAnimationState = LowerBodyAnimationState.IDLE;

        if(animator == null)
        {
            animator = GetComponent<Animator>();
        }

        attackTime = 0;
        isLeftAttack = false;
    }
    void Start()
    {
        attackSpeed = playerStatus.GetAttackSpeed();
    }
    void Update()
    {
        attackTime += Time.deltaTime;
        if (attackTime >= attackSpeed)
        {
            playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
            attackTime = 0;
            // 손 바꿔주기
            isLeftAttack = !isLeftAttack;
        }
    }
    private void OnAnimatorIK(int layerIndex)
    {
        // 잡기
        if (upperBodyAnimationState == UpperBodyAnimationState.HOLD)
        {
            animator.SetIKPositionWeight(AvatarIKGoal.LeftHand, 1f);
            animator.SetIKRotationWeight(AvatarIKGoal.LeftHand, 1f);
            animator.SetIKHintPositionWeight(AvatarIKHint.LeftElbow, 1f);

            animator.SetIKPosition(AvatarIKGoal.LeftHand, targetLeftHand.position);
            animator.SetIKRotation(AvatarIKGoal.LeftHand, targetLeftHand.rotation);
            animator.SetIKHintPosition(AvatarIKHint.LeftElbow, targetLeftHint.position);

            animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
            animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
            animator.SetIKHintPositionWeight(AvatarIKHint.RightElbow, 1f);

            animator.SetIKPosition(AvatarIKGoal.RightHand, targetRightHand.position);
            animator.SetIKRotation(AvatarIKGoal.RightHand, targetRightHand.rotation);
            animator.SetIKHintPosition(AvatarIKHint.LeftElbow, targetRightHint.position);
        }
        // 일반 공격
        else if (upperBodyAnimationState == UpperBodyAnimationState.ATTACK)
        {
            if (isLeftAttack == true)
            {
                if (attackTime < (attackSpeed / 2))
                {
                    animator.SetIKPositionWeight(AvatarIKGoal.LeftHand, 1f);
                    animator.SetIKRotationWeight(AvatarIKGoal.LeftHand, 1f);
                    animator.SetIKHintPositionWeight(AvatarIKHint.LeftElbow, 1f);

                    animator.SetIKPosition(AvatarIKGoal.LeftHand, attackLeftHand.position);
                    animator.SetIKRotation(AvatarIKGoal.LeftHand, attackLeftHand.rotation);
                    animator.SetIKHintPosition(AvatarIKHint.LeftElbow, attackLeftHint.position);
                }
                else
                {
                    animator.SetIKPositionWeight(AvatarIKGoal.LeftHand, 1f);
                    animator.SetIKRotationWeight(AvatarIKGoal.LeftHand, 1f);
                    animator.SetIKHintPositionWeight(AvatarIKHint.LeftElbow, 1f);

                    animator.SetIKPosition(AvatarIKGoal.LeftHand, targetLeftHand.position);
                    animator.SetIKRotation(AvatarIKGoal.LeftHand, targetLeftHand.rotation);
                    animator.SetIKHintPosition(AvatarIKHint.LeftElbow, targetLeftHint.position);
                }
            }
            else
            {
                if (attackTime < (attackSpeed / 2))
                {
                    animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKHintPositionWeight(AvatarIKHint.RightElbow, 1f);

                    animator.SetIKPosition(AvatarIKGoal.RightHand, attackRightHand.position);
                    animator.SetIKRotation(AvatarIKGoal.RightHand, attackRightHand.rotation);
                    animator.SetIKHintPosition(AvatarIKHint.RightElbow, attackRightHint.position);
                }
                else
                {
                    animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKHintPositionWeight(AvatarIKHint.RightElbow, 1f);

                    animator.SetIKPosition(AvatarIKGoal.RightHand, targetRightHand.position);
                    animator.SetIKRotation(AvatarIKGoal.RightHand, targetRightHand.rotation);
                    animator.SetIKHintPosition(AvatarIKHint.LeftElbow, targetRightHint.position);
                }
            }
        }
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void SetUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        this.upperBodyAnimationState = upperBodyAnimationState;
        if(upperBodyAnimationState == UpperBodyAnimationState.NONE)
        {
            animationModule.SetUpdateUpperBodyAnimation(false);
        }
        else
        {
            animationModule.SetUpdateUpperBodyAnimation(true);
        }

        if (upperBodyAnimationState == UpperBodyAnimationState.ATTACK)
        {
            attackTime = 0f;
        }
    }
    public void SetLowerBodyAnimationState(LowerBodyAnimationState lowerBodyAnimationState)
    {
        this.lowerBodyAnimationState = lowerBodyAnimationState;
        SetAnimation();
    }

    private void SetAnimation()
    {
        switch (lowerBodyAnimationState)
        {
            case LowerBodyAnimationState.WALK:
                {
                    ChangeAnimation("Walk");
                    break;
                }
            case LowerBodyAnimationState.RUN:
                {
                    ChangeAnimation("Run");
                    break;
                }
            case LowerBodyAnimationState.IDLE:
                {
                    ChangeAnimation("Idle");
                    break;
                }
        }
    }
    private void ChangeAnimation(string state)
    {
        animator.SetTrigger("New State");
        animator.SetTrigger(state);
    }
}