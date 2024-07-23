using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class AnimationController : MonoBehaviour
{
    // �ִϸ��̼��� LowerBodyAnimationState �������� ����
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    private ActiveRagdoll.AnimationModule animationModule;

    private void Awake()
    {   
        upperBodyAnimationState = UpperBodyAnimationState.NONE;
        lowerBodyAnimationState = LowerBodyAnimationState.IDLE;
    }
    void Start()
    {
        animationModule = this.GetComponent<ActiveRagdoll.AnimationModule>();
    }
    void Update()
    {
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
    private void SetAnimation()
    {
    }
}