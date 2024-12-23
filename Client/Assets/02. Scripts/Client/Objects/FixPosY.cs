using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FixPosY : MonoBehaviour
{
    private float lastPosY;

    private void OnEnable()
    {
        lastPosY = transform.position.y;
    }
    private void FixedUpdate()
    {
        if(transform.position.y > lastPosY)
        {
            transform.position = new Vector3(transform.position.x, lastPosY, transform.position.z);
        }
        lastPosY = transform.position.y;
    }
}
