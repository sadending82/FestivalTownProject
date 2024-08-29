using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    public bool isLeftHand;
    private bool attackWithLeftHand;

    private void Awake()
    {
        isAttackState = false;
        attackWithLeftHand = !isLeftHand;
    }
    private void OnTriggerEnter(Collider other)
    {
        // ������ ���������� ������ ������ Ȯ��
        if(other.gameObject.tag == "HitBox" && isAttackState == true && isLeftHand == attackWithLeftHand)
        {
            // �ǰ� ���� ���ϱ�
            Vector3 direction = other.transform.position - this.transform.position;
            direction.y = 0;

            // ���̾�� �ǰݴ��� �÷��̾ ã�� state�� �ҷ���
            CharacterStatus targetState = Managers.Player.gameObject.transform.GetChild(other.gameObject.layer - 11).GetComponent<CharacterStatus>();
            
            /// <summary>
            /// ���⼭ ������ �ǰݰ��� ������
            /// </summary>
        }
    }
    public void SetIsAttackState(bool isAttackState)
    {
        this.isAttackState = isAttackState;
    }
    public void SetAttackWithLeftHand(bool attackWithLeftHand)
    {
        this.attackWithLeftHand = attackWithLeftHand;
    }
}
