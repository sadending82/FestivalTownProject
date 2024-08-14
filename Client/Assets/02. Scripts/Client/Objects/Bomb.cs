using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    private float lifeTime;
    private float timer = 0;
    public GameObject bombPusher;
    private void OnEnable()
    {
        timer = 0;
    }
    private void OnDisable()
    {
        Rigidbody rigidbody = this.GetComponent<Rigidbody>();
        rigidbody.constraints = RigidbodyConstraints.FreezeRotationX |
                                RigidbodyConstraints.FreezeRotationY |
                                RigidbodyConstraints.FreezeRotationZ;
    }
    private void Update()
    {
        timer += Time.deltaTime;
        if(timer>= lifeTime)
        {
            Boom();
        }
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
    public void Boom()
    {
        GameObject pusher = Instantiate(bombPusher) as GameObject;

        pusher.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, this.transform.position.z);
        pusher.SetActive(true);

        this.gameObject.SetActive(false);
    }
}
