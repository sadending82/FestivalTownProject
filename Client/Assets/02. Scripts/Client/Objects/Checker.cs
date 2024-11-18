using NetworkProtocol;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private PacketManager packetManager;
    private bool groundChecker = false;

    private void Start()
    {
        packetManager = Managers.Network.GetPacketManager();
    }
    private void OnTriggerEnter(Collider other)
    {
        if (groundChecker == false)
        {
            if (other.gameObject.tag == "Bomb")
            {
                if (Managers.Player.GetIsHost() == true)
                {
                    packetManager.SendBombExplosionPacket(other.gameObject.GetComponent<Bomb>().transform.position, other.gameObject.GetComponent<Bomb>().GetId());
                }
            }
            if (other.gameObject.tag == "Weapon" && other.gameObject.GetComponent<Weapon>().GetIsPickUp() == false)
            {
                if (Managers.Player.GetIsHost() == true)
                {
                    packetManager.SendWeaponDeletePacket(other.gameObject.GetComponent<Weapon>().GetId());
                }
            }
            if (other.gameObject.tag == "HitBox")
            {
                if (Managers.Player.GetIsHost() == true)
                {
                    CharacterStatus tPlayerState = other.gameObject.transform.GetComponentInParent<CharacterStatus>();
                    PlayerController tPlayerController = other.gameObject.transform.GetComponentInParent<PlayerController>();

                    Vector3 tPlayerPos = tPlayerController.GetPosition();
                    int mapPosX = (int)transform.position.x / 2;
                    int mapPosZ = (int)transform.position.z / 2;
                    float xInterval = transform.position.x - tPlayerPos.x;
                    float yInterval = transform.position.z - tPlayerPos.z;

                    int mapPosXOffset = 0, mapPosZOffset = 0;
                    float pushPower;
                    if (Mathf.Abs(xInterval) < Mathf.Abs(yInterval))
                    {
                        pushPower = (1.5f - Mathf.Abs(yInterval)) * 150.0f; 
                        if (yInterval > 0) mapPosZOffset = -1;
                        else mapPosZOffset = 1;
                    }
                    else
                    {
                        pushPower = (1.5f - Mathf.Abs(xInterval)) * 150.0f;
                        if (xInterval > 0) mapPosXOffset = -1;
                        else mapPosXOffset = 1;
                    }

                    float tPosMapHeight = Managers.CubeObject.GetMapHeight(mapPosX + mapPosXOffset, mapPosZ + mapPosZOffset);
                    if (tPosMapHeight > tPlayerPos.y)
                    {
                        packetManager.SendPlayerDamageReceivePacket(tPlayerState.GetId(), tPlayerState.GetId(), -1, eDamageType.AT_FALLDOWN, Vector3.zero);
                    }
                    else
                    {
                        packetManager.SendPlayerCollisionToBlockPacket(tPlayerState.GetId());
                        Vector3 pushDirection = new Vector3(mapPosXOffset, 0.0f, mapPosZOffset);
                        tPlayerController.Pushed(pushDirection, pushPower);
                    }
                }
            }
        }
    }
    public void SetGroundChecker(bool groundChecker)
    {
        this.groundChecker = groundChecker;
    }
}
