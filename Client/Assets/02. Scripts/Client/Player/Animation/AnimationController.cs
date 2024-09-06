using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ClientProtocol;

public class AnimationController : MonoBehaviour
{
    // �ִϸ��̼��� ��ü�� ������ ���¸� ��ȯ�ϰ� ��ü�� Ŭ�󿡼� �����Ѵ�.
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    public ActiveRagdoll.AnimationModule animationModule;

    [Header("--- Animation ---")]
    public Animator animator;

    [Header("--- IK ---")]
    // Ÿ��
    public Transform targetLeftHand;
    public Transform targetRightHand;
    public Transform targetLeftHint;
    public Transform targetRightHint;
    // �Ϲ� ���� ����
    public Transform attackLeftHand;
    public Transform attackRightHand;
    public Transform attackLeftHint;
    public Transform attackRightHint;
    // ���� ���� ����
    public Transform WeaponAttackRightHand;
    public Transform WeaponAttackRightHint;

    public CharacterStatus playerStatus;
    private float attackSpeed;
    private float attackTimer;
    private bool isLeftAttack;
    private float weaponAttackTimer;
    private float weaponAttackSpeed;

    public AttackChecker leftAttackChecker;
    public AttackChecker rightAttackChecker;

    private void Awake()
    {   
        upperBodyAnimationState = UpperBodyAnimationState.NONE;
        lowerBodyAnimationState = LowerBodyAnimationState.IDLE;

        if(animator == null)
        {
            animator = GetComponent<Animator>();
        }

        attackTimer = 0;
        weaponAttackTimer = 0;
        isLeftAttack = false;
    }
    void Start()
    {
        attackSpeed = playerStatus.GetAttackSpeed();
        weaponAttackSpeed = attackSpeed * 2;
    }
    void Update()
    {
        if (upperBodyAnimationState == UpperBodyAnimationState.ATTACK)
        {
            attackTimer += Time.deltaTime;
            if (attackTimer >= attackSpeed)
            {
                playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                attackTimer = 0;
                // �� �ٲ��ֱ�
                isLeftAttack = !isLeftAttack;
                leftAttackChecker.SetIsAttackState(false);
                rightAttackChecker.SetIsAttackState(false);
            }
        }
        else if(upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
        {
            weaponAttackTimer += Time.deltaTime;
            if(weaponAttackTimer >= weaponAttackSpeed)
            {
                playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                weaponAttackTimer = 0;
            }
        }
    }
    private void OnAnimatorIK(int layerIndex)
    {
        // ���
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
        // �Ϲ� ����
        else if (upperBodyAnimationState == UpperBodyAnimationState.ATTACK)
        {
            if (isLeftAttack == true)
            {
                if (attackTimer < (attackSpeed / 2))
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
                    leftAttackChecker.SetIsAttackState(true);

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
                if (attackTimer < (attackSpeed / 2))
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
                    rightAttackChecker.SetIsAttackState(true);

                    animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
                    animator.SetIKHintPositionWeight(AvatarIKHint.RightElbow, 1f);

                    animator.SetIKPosition(AvatarIKGoal.RightHand, targetRightHand.position);
                    animator.SetIKRotation(AvatarIKGoal.RightHand, targetRightHand.rotation);
                    animator.SetIKHintPosition(AvatarIKHint.LeftElbow, targetRightHint.position);
                }
            }
        }
        else if(upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
        {
            if (weaponAttackTimer < (weaponAttackSpeed / 2))
            {
                animator.SetIKPositionWeight(AvatarIKGoal.RightHand, 1f);
                animator.SetIKRotationWeight(AvatarIKGoal.RightHand, 1f);
                animator.SetIKHintPositionWeight(AvatarIKHint.RightElbow, 1f);

                animator.SetIKPosition(AvatarIKGoal.RightHand, WeaponAttackRightHand.position);
                animator.SetIKRotation(AvatarIKGoal.RightHand, WeaponAttackRightHand.rotation);
                animator.SetIKHintPosition(AvatarIKHint.RightElbow, WeaponAttackRightHint.position);
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

    // ------------------- GETTERS & SETTERS -------------------
    public void SetUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        this.upperBodyAnimationState = upperBodyAnimationState;
        if(upperBodyAnimationState == UpperBodyAnimationState.NONE)
        {
            leftAttackChecker.SetIsAttackState(false);
            rightAttackChecker.SetIsAttackState(false);

            animationModule.SetUpdateUpperBodyAnimation(false);
        }
        else
        {
            animationModule.SetUpdateUpperBodyAnimation(true);
        }

        if (upperBodyAnimationState == UpperBodyAnimationState.ATTACK)
        {
            attackTimer = 0f;
        }
        else if (upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
        {
            weaponAttackTimer = 0f;
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