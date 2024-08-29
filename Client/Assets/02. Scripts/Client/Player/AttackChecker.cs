using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    public bool isLeftHand;
    private bool attackWithLeftHand;

    public GameObject pelvis;

    public CharacterStatus playerState;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;

    private void Awake()
    {
        isAttackState = false;
        attackWithLeftHand = !isLeftHand;
    }
    private void Start()
    {
        network = Managers.Network;
        packetManager = network.GetPacketManager();
    }
    private void OnTriggerEnter(Collider other)
    {
        // ������ ���������� ������ ������ Ȯ��
        if(other.gameObject.tag == "HitBox" && isAttackState == true && isLeftHand == attackWithLeftHand)
        {
            Debug.Log("�� �ƾ�!!");

            // �ǰ� ���� ���ϱ�
            Vector3 direction = other.transform.position - pelvis.transform.position;
            direction.y = 0;

            //// ���̾�� �ǰݴ��� �÷��̾ ã�� state�� �ҷ���
            //CharacterStatus targetState = Managers.Player.gameObject.transform.GetChild(other.gameObject.layer - 11).GetComponent<CharacterStatus>();
            //targetState.Attacked(direction.normalized, 20);

            /// <summary>
            /// ���⼭ ������ �ǰݰ��� ������
            /// </summary>
            packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), other.gameObject.layer - 11, 1, 1, direction.normalized);
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
