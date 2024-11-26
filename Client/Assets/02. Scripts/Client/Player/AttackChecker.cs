using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using Unity.VisualScripting;
using static UnityEngine.GraphicsBuffer;
using UnityEngine.UIElements;

public class AttackChecker : MonoBehaviour
{
    private bool isAttackState;
    private bool isGrapState;
    public bool isLeftHand;

    public CharacterStatus playerState;
    public PlayerController playerController;
    public Transform effectPoint;

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
        // 때리는 상태인지와 때리는 중인지
        if (other.gameObject.tag == "HitBox" && isAttackState == true && other.gameObject.layer != this.gameObject.layer)
        {
            if (other.gameObject.transform.parent.GetComponent<CharacterStatus>().GetTeamNumber() != playerState.GetTeamNumber())
            {
                Managers.Effect.PlayEffect("Attacked", effectPoint.position);

                if (playerState.GetAmIPlayer() == true)
                {
                    isAttackState = false;

                    // 피격 방향 구하기
                    Vector3 attackedDirection = playerState.GetAttackedDirection(other.transform.position);

                    int targetId = other.transform.GetComponentInParent<CharacterStatus>().GetId();

                    packetManager.SendPlayerDamageReceivePacket(playerState.GetId(), targetId, (int)eWeaponType.WT_HAND, eDamageType.AT_ATTACK, attackedDirection);
                }
            }
        }
        if (playerState.GetAmIPlayer() == true)
        {
            if (other.gameObject.tag == "Head" && isGrapState == true && other.gameObject.layer != this.gameObject.layer)
            {
                CharacterStatus targetPlayerState = other.transform.GetComponent<LinkRootGameObject>().GetRootGameObject().GetComponent<CharacterStatus>();
                if (targetPlayerState.GetIsGroggy() == true)
                {
                    Rigidbody rb = other.transform.GetComponent<Rigidbody>();
                    if (rb != null)
                    {
                        Debug.Log("Grap!!");
                        isGrapState = false;

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
    public void ThrowPlayer()
    {
        Destroy(this.gameObject.GetComponent<FixedJoint>());
    }
}
