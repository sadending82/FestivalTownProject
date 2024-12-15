using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class GroggyUI : MonoBehaviour
{
    private const float SHAKE_DEGREE = 15.0f;
    private float shakeSpeed = 60.0f;
    private float zAngle = 0.0f;

    private void Update()
    {
        zAngle += shakeSpeed * Time.deltaTime;
        if(shakeSpeed > 0.0f && zAngle >= SHAKE_DEGREE)
        {
            zAngle = SHAKE_DEGREE;
            shakeSpeed *= -1;
        }
        else if (shakeSpeed < 0.0f && zAngle <= -SHAKE_DEGREE)
        {
            zAngle = -SHAKE_DEGREE;
            shakeSpeed *= -1;
        }
        transform.eulerAngles = new Vector3 (transform.eulerAngles.x, transform.eulerAngles.y , zAngle);
    }
}