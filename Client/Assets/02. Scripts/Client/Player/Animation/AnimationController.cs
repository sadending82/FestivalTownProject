using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class AnimationController : MonoBehaviour
{
    // �ִϸ��̼��� LowerBodyAnimationState �������� ����
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    private CharacterStatus characterStatus;

    void Start()
    {
        characterStatus = this.GetComponent<CharacterStatus>();
    }
    void Update()
    {
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void setUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        if (upperBodyAnimationState == UpperBodyAnimationState.NONE)
        {
            this.upperBodyAnimationState = upperBodyAnimationState;
            SetUseIKAndAnimationModule();
            SetAnimation();
        }
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
                    characterStatus.SetUseIK(true);
                    break;
                }
            default:
                {
                    characterStatus.SetUseIK(false);
                    break;
                }
        }
        switch (lowerBodyAnimationState)
        {
            case LowerBodyAnimationState.JUMP:
                {
                    characterStatus.SetUseAnimationModule(false);
                    break;
                }
            case LowerBodyAnimationState.ROLL:
            case LowerBodyAnimationState.FLYINGKICK:
                {
                    characterStatus.SetUseIK(false);
                    characterStatus.SetUseAnimationModule(false);
                    break;
                }
            default:
                {
                    characterStatus.SetUseAnimationModule(true);
                    break;
                }
        }
    }
    private void SetAnimation()
    {
    }
}