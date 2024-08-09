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
        // 바닥에 도달했을때 더이상 움직이지 않도록 고정
        if (other.gameObject.tag == "Player" && check == false)
        {
            check = true;

            // Pusher 작동
            pusher.SetActive(true);
            pusher.transform.position = new Vector3(this.transform.position.x, 5, this.transform.position.z);
        }
    }
}
