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
            // 때리는 상태인지와 때리는 중인지
            if (other.gameObject.tag == "HitBox" && isAttackState == true)
            {
                // 피격 방향 구하기
                Vector3 direction = other.transform.position - pelvis.transform.position;
                direction.y = 0;

                int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                Debug.Log("나 " + playerState.GetId() + " 인데 얘 " + targetId + " 쳤어");

                /// <summary>
                /// 여기서 서버로 피격관련 전해줌
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
