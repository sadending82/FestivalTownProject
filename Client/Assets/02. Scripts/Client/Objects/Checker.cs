using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private bool check;

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
                if (Managers.Player.GetIsHost() == true)
                {
                    CharacterStatus tPlayerState = other.gameObject.GetComponent<CharacterStatus>();
                    packetManager.SendPlayerCollisionToBlockPacket(tPlayerState.GetId());
                }
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
            if (other.gameObject.tag == "Ground")
            {
                check = false;
                GameObject pusher = Managers.Resource.Instantiate("CubePusher");
                // Pusher ¿€µø
                pusher.transform.position = new Vector3(this.transform.position.x, 0, this.transform.position.z);
                pusher.SetActive(true);
            }
        }
    }

    public void SetCheck(bool check)
    {
        this.check = check;
    }
}
