using NetworkProtocol;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pusher : MonoBehaviour
{
    public float maxScale = 3;
    public float pushSpeed = 0.5f;
    public bool usePushY = false;
    private bool isStartPush = false;

    private PacketManager packetManager;

    private void OnEnable()
    {
        packetManager = Managers.Network.GetPacketManager();
    }
    public void StartPush()
    {
        isStartPush = true;
    }
    private void FixedUpdate()
    {
        if (isStartPush == true)
        {
            GetBigger();
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "HitBox")
        {
            CharacterStatus tPlayerState = collision.transform.GetComponentInParent<CharacterStatus>();

            if (tPlayerState.GetAmIPlayer() == true)
            {
                // 큐브가 플레이어 밀어낼 때 -> 다른 곳에도 Pusher 쓸거면 수정해야함
                if (usePushY == false)
                {
                    packetManager.SendPlayerCollisionToBlockPacket(tPlayerState.GetId());
                }
                // 폭탄이 플레이어 밀어낼 때 -> 다른 곳에도 Pusher 쓸거면 수정해야함
                else
                {
                    packetManager.SendPlayerDamageReceivePacket(tPlayerState.GetId(), tPlayerState.GetId(), -1, eDamageType.AT_BOMB_ATTACK, Vector3.zero);
                }
            }
        }
    }
    private void GetBigger()
    {
        if (usePushY == true)
        {
            this.transform.localScale += new Vector3(pushSpeed, pushSpeed, pushSpeed);
        }
        else
        {
            this.transform.localScale += new Vector3(pushSpeed, 0, pushSpeed);
        }
        if(this.transform.localScale.x >= maxScale)
        {
            DeletePusher();
        }
    }
    private void DeletePusher()
    {
        isStartPush = false;
        if (usePushY == true)
        {
            this.transform.localScale = Vector3.zero;
        }
        else
        {
            this.transform.localScale = new Vector3(0, this.transform.localScale.y, 0);
        }
        this.gameObject.SetActive(false);
        Managers.Resource.Destroy(this.gameObject);
    }
}
