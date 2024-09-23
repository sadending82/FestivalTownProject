using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    public bool isLeftHand;

    public CharacterStatus playerState;

    public bool amIPlayer;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;

    private void Awake()
    {
        isAttackState = false;
    }
    private void Start()
    {
        network = Managers.Network;
        packetManager = network.GetPacketManager();

        amIPlayer = playerState.GetAmIPlayer();
    }
    private void OnTriggerEnter(Collider other)
    {
        if (amIPlayer == true)
        {
            // ������ ���������� ������ ������
            if (other.gameObject.tag == "HitBox" && isAttackState == true)
            {
                // �ǰ� ���� ���ϱ�
                Vector3 attackedDirection = playerState.GetAttackedDirection(other.transform.position);

                int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                Debug.Log("�� " + playerState.GetId() + " �ε� �� " + targetId + " �ƾ�");

                packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), targetId, (int)eWeaponType.WT_HAND, eDamageType.AT_BASIC_ATTACK, attackedDirection);
            }
        }
    }
    public void SetIsAttackState(bool isAttackState)
    {
        this.isAttackState = isAttackState;
    }
}
