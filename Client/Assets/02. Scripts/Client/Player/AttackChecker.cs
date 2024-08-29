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
        // 때리는 상태인지와 때리는 손인지 확인
        if(other.gameObject.tag == "HitBox" && isAttackState == true && isLeftHand == attackWithLeftHand)
        {
            // 피격 방향 구하기
            Vector3 direction = other.transform.position - this.transform.position;
            direction.y = 0;

            // 레이어로 피격당한 플레이어를 찾고 state를 불러옴
            CharacterStatus targetState = Managers.Player.gameObject.transform.GetChild(other.gameObject.layer - 11).GetComponent<CharacterStatus>();
            
            /// <summary>
            /// 여기서 서버로 피격관련 전해줌
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
