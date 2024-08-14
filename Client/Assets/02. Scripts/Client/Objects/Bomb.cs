using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    private float lifeTime;
    private void OnEnable()
    {
        
    }
    private void OnDisable()
    {

    }

    private void OnCollisionEnter(Collision collision)
    {
        // 바닥에 도달했을때 더이상 움직이지 않도록 고정
        if (collision.gameObject.tag == "Ground")
        {
            Rigidbody rigidbody = this.GetComponent<Rigidbody>();
            rigidbody.constraints = RigidbodyConstraints.FreezePositionX |
                                    RigidbodyConstraints.FreezePositionY |
                                    RigidbodyConstraints.FreezePositionZ |
                                    RigidbodyConstraints.FreezeRotationX |
                                    RigidbodyConstraints.FreezeRotationY |
                                    RigidbodyConstraints.FreezeRotationZ;
        }
    }
    public void SetLifeTime(float lifeTime)
    {
        this.lifeTime = lifeTime;
    }
}
