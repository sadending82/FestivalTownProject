using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pusher : MonoBehaviour
{
    public float maxScale = 3;
    public float pushSpeed = 0.5f;
    public bool usePushY = false;
    private bool isStartPush = false;

    private void OnEnable()
    {
        StartPush();
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
    }
}
