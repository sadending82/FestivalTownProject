using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class AnimationController : MonoBehaviour
{
    // 애니메이셔은 LowerBodyAnimationState 기준으로 관리
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
        // upperBodyAnimationState와 lowerBodyAnimationState의 상태에 따라 IK와 Animation을 사용할지에 대한 코드
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