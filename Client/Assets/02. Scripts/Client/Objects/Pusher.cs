using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pusher : MonoBehaviour
{
    public float maxScale = 3;
    public float pushSpeed = 0.5f;
    private bool isStartPush = false;

    public void StartPush()
    {
        isStartPush = true;
        this.transform.localScale = Vector3.zero;
    }
    private void FixedUpdate()
    {
        if (isStartPush == true)
        {
            GetBigger();
        }
    }
    private void GetBigger()
    {
        this.transform.localScale += new Vector3(pushSpeed, pushSpeed, pushSpeed);
        if(this.transform.localScale.x >= maxScale)
        {
            DeletePusher();
        }
    }
    private void DeletePusher()
    {
        isStartPush = false;
        this.transform.localScale = Vector3.zero;
        Managers.Resource.Destroy(this.gameObject);
    }
}
