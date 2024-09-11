using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube : MonoBehaviour
{
    private float targetHeight;
    private bool heightChecker = false;

    public Checker checker;

    private void Start()
    {
        if(checker == null)
        {
            checker = this.transform.GetChild(0).GetComponent<Checker>();
        }
    }
    private void FixedUpdate()
    {
        if(heightChecker == false)
        {
            if(this.transform.position.y<targetHeight)
            {
                this.transform.position = new Vector3(this.transform.position.x, targetHeight, this.transform.position.z);
                heightChecker = true;
            }
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        // 바닥에 도달했을때 더이상 움직이지 않도록 고정
        if(collision.gameObject.tag == "Ground")
        {
            this.transform.position = new Vector3(this.transform.position.x, targetHeight, this.transform.position.z);
            heightChecker = true;
            Rigidbody rigidbody = this.GetComponent<Rigidbody>();
            rigidbody.constraints = RigidbodyConstraints.FreezePositionX |
                                    RigidbodyConstraints.FreezePositionY |
                                    RigidbodyConstraints.FreezePositionZ |
                                    RigidbodyConstraints.FreezeRotationX |
                                    RigidbodyConstraints.FreezeRotationY |
                                    RigidbodyConstraints.FreezeRotationZ;
            checker.SetCheck(false);

            int soundNum = Random.Range(1, 5);
            Managers.Sound.Play3D($"Sfx_Block_Impact{soundNum}", gameObject);
        }
    }
    public void SetTargetHeight(float targetHeight)
    {
        this.targetHeight = targetHeight;
    }
}