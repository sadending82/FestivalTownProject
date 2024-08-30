using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    public bool isLeftHand;

    public GameObject pelvis;
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
                Vector3 direction = other.transform.position - pelvis.transform.position;
                direction.y = 0;

                int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                Debug.Log("�� " + playerState.GetId() + " �ε� �� " + targetId + " �ƾ�");

                /// <summary>
                /// ���⼭ ������ �ǰݰ��� ������
                /// </summary>
                packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), targetId, 1, 1, direction.normalized);
            }
        }
    }
    public void SetIsAttackState(bool isAttackState)
    {
        this.isAttackState = isAttackState;
    }
}
