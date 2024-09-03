using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private bool check;
    public GameObject pusher;
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

                // Pusher ¿€µø
                pusher.SetActive(true);
                pusher.transform.position = new Vector3(this.transform.position.x, 5, this.transform.position.z);
            }
            if (other.gameObject.tag == "Bomb")
            {
                if (Managers.Player.GetIsHost() == true)
                {
                    Managers.Network.GetPacketManager().SendBombExplosionPacket(other.gameObject.GetComponent<Bomb>().transform.position, other.gameObject.GetComponent<Bomb>().GetId());
                    //other.gameObject.GetComponent<Bomb>().Boom();
                }
            }
        }
    }

    public void SetCheck(bool check)
    {
        this.check = check;
    }
}
