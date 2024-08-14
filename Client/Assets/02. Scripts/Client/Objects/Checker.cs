using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Checker : MonoBehaviour
{
    private bool check;
    public GameObject pusher;
    private void OnEnable()
    {
        check = false;
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == "Player" && check == false)
        {
            check = true;

            // Pusher ¿€µø
            pusher.SetActive(true);
            pusher.transform.position = new Vector3(this.transform.position.x, 5, this.transform.position.z);
        }
        if (other.gameObject.tag == "Bomb")
        {
            other.gameObject.GetComponent<Bomb>().Boom();
        }
    }
}
