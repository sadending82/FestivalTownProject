using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private bool check;

    private PacketManager packetManager;

    private GameObject pusher;

    private void Start()
    {
        check = true;
        packetManager = Managers.Network.GetPacketManager();
    }
    public void CreatePusher()
    {
        pusher = Managers.Resource.Instantiate("CubePusher");
        pusher.transform.position = new Vector3(this.transform.position.x, 0, this.transform.position.z);
    }
    private void OnTriggerEnter(Collider other)
    {
        if (check == true)
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
            if (other.gameObject.tag == "Ground")
            {
                check = false;
                pusher.SetActive(true);
                pusher.GetComponent<Pusher>().StartPush();
            }
        }
    }

    public void SetCheck(bool check)
    {
        this.check = check;
    }
}
