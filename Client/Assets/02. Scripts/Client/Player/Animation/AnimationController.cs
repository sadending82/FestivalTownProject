using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class AnimationController : MonoBehaviour
{
    // �ִϸ��̼��� LowerBodyAnimationState �������� ����
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    public ActiveRagdoll.AnimationModule animationModule;

    [Header("--- Animation ---")]
    public Animator animator;

    [Header("--- IK ---")]
    public Transform leftHandTarget;
    public Transform rightHandTarget;

    private void Awake()
    {   
        upperBodyAnimationState = UpperBodyAnimationState.NONE;
        lowerBodyAnimationState = LowerBodyAnimationState.IDLE;

        if(animator == null)
        {
            animator = GetComponent<Animator>();
        }
    }
    void Start()
    {
    }
    void Update()
    {
    }
    private void OnAnimatorIK(int layerIndex)
    {
        // test
        if (upperBodyAnimationState == UpperBodyAnimationState.HOLD)
        {
            animator.SetIKPositionWeight(AvatarIKGoal.LeftHand, 1f);
            animator.SetIKRotationWeight(AvatarIKGoal.LeftHand, 1f);
        
            animator.SetIKPosition(AvatarIKGoal.LeftHand, leftHandTarget.position);
            animator.SetIKRotation(AvatarIKGoal.LeftHand, leftHandTarget.rotation);
        
            animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
            animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
        
            animator.SetIKPosition(AvatarIKGoal.RightHand, rightHandTarget.position);
            animator.SetIKRotation(AvatarIKGoal.RightHand, rightHandTarget.rotation);
        }
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void setUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        this.upperBodyAnimationState = upperBodyAnimationState;
        SetUseIKAndAnimationModule();
        SetAnimation();
    }
    public void setLowerBodyAnimationState(LowerBodyAnimationState lowerBodyAnimationState)
    {
        if (lowerBodyAnimationState != LowerBodyAnimationState.ROLL &&
            lowerBodyAnimationState != LowerBodyAnimationState.FLYINGKICK)
        {
            this.lowerBodyAnimationState = lowerBodyAnimationState;
            SetUseIKAndAnimationModule();
            SetAnimation();
        }
    }
    private void SetUseIKAndAnimationModule()
    {
        // upperBodyAnimationState�� lowerBodyAnimationState�� ���¿� ���� IK�� Animation�� ��������� ���� �ڵ�
        switch (upperBodyAnimationState)
        {
            case UpperBodyAnimationState.ATTACK:
            case UpperBodyAnimationState.POWERATTACK:
            case UpperBodyAnimationState.HOLD:
            case UpperBodyAnimationState.THROW:
            case UpperBodyAnimationState.HEADATTACK:
                {
                    animationModule.SetUseIK(true);
                    break;
                }
            default:
                {
                    animationModule.SetUseIK(false);
                    break;
                }
        }
        switch (lowerBodyAnimationState)
        {
            case LowerBodyAnimationState.JUMP:
                {
                    animationModule.SetUseAnimationModule(false);
                    break;
                }
            case LowerBodyAnimationState.ROLL:
            case LowerBodyAnimationState.FLYINGKICK:
                {
                    animationModule.SetUseIK(false);
                    animationModule.SetUseAnimationModule(false);
                    break;
                }
            default:
                {
                    animationModule.SetUseAnimationModule(true);
                    break;
                }
        }
    }
    // �ִϸ��̼� ���¿� ���� �ѹ� ����
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
            case LowerBodyAnimationState.ROLL:
            case LowerBodyAnimationState.FLYINGKICK:
                {
                    break;
                }
            default:
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