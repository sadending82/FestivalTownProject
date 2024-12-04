using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;

public class FlyingKickChecker : MonoBehaviour
{
    private bool isAttackState;

    public CharacterStatus playerState;
    public PlayerController playerController;

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
    }
    private void OnCollisionEnter(Collision other)
    {
        // ������ ���������� ������ ������
        if (other.gameObject.tag == "HitBox" && isAttackState == true && other.gameObject.layer != this.gameObject.layer)
        {
            if (other.gameObject.transform.parent.GetComponent<CharacterStatus>().GetTeamNumber() != playerState.GetTeamNumber())
            {
                Managers.Effect.PlayEffect("Attacked", transform.position);

                if (playerState.GetAmIPlayer() == true)
                {
                    isAttackState = false;

                    // �ǰ� ���� ���ϱ�
                    Vector3 attackedDirection = playerState.GetAttackedDirection(other.transform.position);

                    int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                    packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), targetId, (int)eWeaponType.WT_NOTHING, eDamageType.AT_JUMP_ATTACK, attackedDirection);
                }
            }
        }
    }
    public void SetIsAttackState(bool isAttackState)
    {
        this.isAttackState = isAttackState;
    }
}
