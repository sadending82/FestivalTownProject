using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using Unity.VisualScripting;
using UnityEditor.UIElements;
using static UnityEngine.GraphicsBuffer;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    private bool isGrapState;
    public bool isLeftHand;

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
        if (playerState.GetAmIPlayer() == true)
        {
            // 때리는 상태인지와 때리는 중인지
            if (other.gameObject.tag == "HitBox" && isAttackState == true && other.gameObject.layer != this.gameObject.layer)
            {
                // 피격 방향 구하기
                Vector3 attackedDirection = playerState.GetAttackedDirection(other.transform.position);

                int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                //Debug.Log("나 " + playerState.GetId() + " 인데 얘 " + targetId + " 쳤어");

                packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), targetId, (int)eWeaponType.WT_HAND, eDamageType.AT_BASIC_ATTACK, attackedDirection);
            }
            if(other.gameObject.tag == "Head" && isGrapState == true && other.gameObject.layer != this.gameObject.layer)
            {
                CharacterStatus targetPlayerState = other.transform.GetComponent<LinkRootGameObject>().GetRootGameObject().GetComponent<CharacterStatus>();
                if (targetPlayerState.GetIsGroggy() == true)
                {
                    Rigidbody rb = other.transform.GetComponent<Rigidbody>();
                    if (rb != null)
                    {
                        int targetId = targetPlayerState.GetId();

                        packetManager.SendPlayerGrabOtherPlayerPacket(playerState.GetId(), playerController.GetPosition(), playerController.GetDirection(),
                            this.transform.position, isLeftHand, targetId, other.transform.position);
                    }
                }
            }
        }
    }
    public void SetIsAttackState(bool isAttackState)
    {
        this.isAttackState = isAttackState;
    }
    public void SetIsGrapState(bool isGrapState)
    {
        this.isGrapState = isGrapState;
        if(isGrapState == false)
        {
            Destroy(this.gameObject.GetComponent<FixedJoint>());
        }
    }
    public void GrapPlayer(int targetId, Vector3 handPos, Vector3 targetHeadPos)
    {
        PlayerController targetPlayerController = Managers.Player.FindPlayerById(targetId).GetComponent<PlayerController>();
        targetPlayerController.SetHeadPosition(targetHeadPos);
        this.transform.position = handPos;

        FixedJoint fixedJoint = GetComponent<FixedJoint>();
        if (fixedJoint == null)
        {
            fixedJoint = this.gameObject.AddComponent<FixedJoint>() as FixedJoint;
        }
        fixedJoint.connectedBody = targetPlayerController.GetHeadRigidbody();
    }
}
