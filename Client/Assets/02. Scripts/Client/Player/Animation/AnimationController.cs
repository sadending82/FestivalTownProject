using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ClientProtocol;
using ExcelDataStructure;

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
    public Transform targetPoint;
    // �Ϲ� ���� ����
    public Transform attackLeftHand;
    public Transform attackRightHand;
    public Transform attackLeftHint;
    public Transform attackRightHint;
    public GameObject leftHand;
    public GameObject rightHand;
    private float punchMotionPower = 20.0f;
    // ���� ���� ����
    public Transform WeaponAttackRightHand;
    public Transform WeaponAttackRightHint;

    public CharacterStatus playerStatus;

    private float attackSpeed;
    private float attackTime = 0.5f;
    private float weaponAttackSpeed;
    private float attackTimer;
    private bool isLeftAttack;
    private bool oneTimeSendChecker;

    private GameObject myWeapon;

    public AttackChecker leftAttackChecker;
    public AttackChecker rightAttackChecker;

    private bool isGameEnd;

    private void Awake()
    {   
        upperBodyAnimationState = UpperBodyAnimationState.NONE;
        lowerBodyAnimationState = LowerBodyAnimationState.IDLE;
        oneTimeSendChecker = false;

        if (animator == null)
        {
            animator = GetComponent<Animator>();
        }

        attackTimer = 0;
        isLeftAttack = false;
    }
    void Start()
    {
        GameDataEntity data;
        CharacterAttackEntity cse;

        // Attack
        data = Managers.Data.GetData(10001);
        cse = (CharacterAttackEntity)data;
        attackSpeed = cse.Attack_Speed;
        if(attackSpeed <= attackTime)
        {
            attackSpeed = attackTime + 0.1f;
        }

        // Index�� �����ؾ���
        // WeaponAttack
        data = Managers.Data.GetData(10001);
        cse = (CharacterAttackEntity)data;
        weaponAttackSpeed = cse.Attack_Speed;

        isGameEnd = false;
    }
    void Update()
    {
        if (isGameEnd == false)
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
            else if (upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
            {
                attackTimer += Time.deltaTime;
                if (attackTimer >= weaponAttackSpeed)
                {
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                    attackTimer = 0;
                    myWeapon.GetComponent<Weapon>().SetIsAttackState(false);
                }
            }
        }
    }
    private void OnAnimatorIK(int layerIndex)
    {
        if (isGameEnd == false)
        {
            // ���
            if (upperBodyAnimationState == UpperBodyAnimationState.GRAP)
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
                    if (attackTimer < (attackSpeed - attackTime))
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
                        if (oneTimeSendChecker == true)
                        {
                            Managers.Sound.Play("Sfx_Ch_Attack_Motion");
                            Vector3 dir = targetPoint.position - leftAttackChecker.transform.position;
                            dir = dir.normalized;
                            leftHand.GetComponent<Rigidbody>().AddForce(dir * punchMotionPower, ForceMode.Impulse);
                            oneTimeSendChecker = false;
                            leftAttackChecker.SetIsAttackState(true);
                        }
                    }
                }
                else
                {
                    if (attackTimer < (attackSpeed - attackTime))
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
                        if (oneTimeSendChecker == true)
                        {
                            Managers.Sound.Play("Sfx_Ch_Attack_Motion");
                            Vector3 dir = targetPoint.position - rightAttackChecker.transform.position;
                            dir = dir.normalized;
                            rightHand.GetComponent<Rigidbody>().AddForce(dir * punchMotionPower, ForceMode.Impulse);
                            oneTimeSendChecker = false;
                            rightAttackChecker.SetIsAttackState(true);
                        }
                    }
                }
            }
            else if (upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
            {
                if (attackTimer < (weaponAttackSpeed / 3 * 2))
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
                    if (oneTimeSendChecker == true)
                    {
                        Managers.Sound.Play("Sfx_Ch_Weapon_Motion");
                        oneTimeSendChecker = false;
                        Vector3 dir = targetPoint.position - rightAttackChecker.transform.position;
                        dir = dir.normalized;
                        rightHand.GetComponent<Rigidbody>().AddForce(dir * punchMotionPower, ForceMode.Impulse);
                        myWeapon.GetComponent<Weapon>().SetIsAttackState(true);
                    }
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
            oneTimeSendChecker = true;
            attackTimer = 0f;
        }
        else if (upperBodyAnimationState == UpperBodyAnimationState.WEAPONATTACK)
        {
            oneTimeSendChecker = true;
            attackTimer = 0f;
            myWeapon = playerStatus.GetMyWeapon();
        }
        else if (upperBodyAnimationState == UpperBodyAnimationState.GRAP)
        {
            leftAttackChecker.SetIsGrapState(true);
            rightAttackChecker.SetIsGrapState(true);
        }
        else
        {
            leftAttackChecker.SetIsAttackState(false);
            rightAttackChecker.SetIsAttackState(false);

            leftAttackChecker.SetIsGrapState(false);
            rightAttackChecker.SetIsGrapState(false);
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
        animator.SetTrigger(state);
    }
    public void GameStart()
    {
        isGameEnd = false;
    }
    public void GameEnd()
    {
        isGameEnd = true;
    }
}