using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class TestIKAnimationController : MonoBehaviour
{
    [Header("--- Animation ---")]
    public Animator animator;

    [Header("--- IK ---")]
    public Transform leftHandTarget;
    public Transform rightHandTarget;

    private void Awake()
    {
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
        //if (upperBodyAnimationState == UpperBodyAnimationState.HOLD)
        {
            animator.SetIKPositionWeight(AvatarIKGoal.LeftHand, 0.5f);
            animator.SetIKRotationWeight(AvatarIKGoal.LeftHand, 0.5f);

            animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 0.5f);
            animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 0.5f);

            animator.SetIKPosition(AvatarIKGoal.LeftHand, leftHandTarget.position);
            animator.SetIKRotation(AvatarIKGoal.LeftHand, leftHandTarget.rotation);

            animator.SetIKPosition(AvatarIKGoal.RightHand, rightHandTarget.position);
            animator.SetIKRotation(AvatarIKGoal.RightHand, rightHandTarget.rotation);
        }
    }
}