using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private bool check;
    public GameObject pusher;

    private PacketManager packetManager;
    private void Start()
    {
        packetManager = Managers.Network.GetPacketManager();
    }
    private void OnEnable()
    {
        check = true;
    }
    private void OnTriggerEnter(Collider other)
    {
        if (check == true)
        {
            if (other.gameObject.tag == "Player")
            {
                check = false;

                if (Managers.Player.GetIsHost() == true)
                {
                    CharacterStatus tPlayerState = other.gameObject.GetComponent<CharacterStatus>();
                    packetManager.SendPlayerCollisionToBlockPacket(tPlayerState.GetId());
                }

                // Pusher ¿€µø
                pusher.SetActive(true);
                pusher.transform.position = new Vector3(this.transform.position.x, 5, this.transform.position.z);
            }
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
        }
    }

    public void SetCheck(bool check)
    {
        this.check = check;
    }
}
