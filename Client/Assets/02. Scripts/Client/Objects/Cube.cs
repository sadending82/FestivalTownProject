using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube : MonoBehaviour
{
    private void OnCollisionEnter(Collision collision)
    {
        // �ٴڿ� ���������� ���̻� �������� �ʵ��� ����
        if(collision.gameObject.tag == "Ground")
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
}