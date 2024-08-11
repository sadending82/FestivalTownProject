using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube : MonoBehaviour
{
    private float targetHeight;
    private bool heightChecker = false;

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
        // �ٴڿ� ���������� ���̻� �������� �ʵ��� ����
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
        }
    }
    public void SetTargetHeight(float targetHeight)
    {
        this.targetHeight = targetHeight;
    }
}