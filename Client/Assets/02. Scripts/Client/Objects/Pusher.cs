using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pusher : MonoBehaviour
{
    public float maxScale = 3;
    public float pushSpeed = 0.5f;
    private bool isStartPush;

    private void Awake()
    {
        isStartPush = false;
    }
    private void OnEnable()
    {
        this.transform.localScale = new Vector3(0, this.transform.localScale.y, 0);
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
        this.transform.localScale = new Vector3(this.transform.localScale.x + pushSpeed,
                                                this.transform.localScale.y,
                                                this.transform.localScale.z + pushSpeed);
        if(this.transform.localScale.x >= maxScale)
        {
            isStartPush = false;
            Managers.Resource.Destroy(this.gameObject);
        }
    }
}
