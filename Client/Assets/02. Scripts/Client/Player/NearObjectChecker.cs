using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NearObjectChecker : MonoBehaviour
{
    public GameObject nearObject;

    private void Awake()
    {
        nearObject = null;
    }

    private void Update()
    {
        // ������ �ִٰ� �����ߴ� ������Ʈ�� ����� ��
        if (nearObject != null && nearObject.activeSelf == false)
        {
            nearObject = null;
        }
    }

    private void OnTriggerStay(Collider other)
    {
        if (other.gameObject != nearObject)
        {
            if (other.tag == "Bomb")
            {
                Bomb targetBomb = other.GetComponent<Bomb>();
                if (targetBomb.GetIsPickUp() == false)
                {
                    if (nearObject == null)
                    {
                        nearObject = other.gameObject;
                    }
                    else
                    {
                        // other�� ���� nearObject���� �����ٸ� ��ü
                        if (Vector3.Distance(this.transform.position, other.transform.position) < Vector3.Distance(this.transform.position, nearObject.transform.position))
                        {
                            nearObject = other.gameObject;
                        }
                    }
                }
            }
        }
    }
    private void OnTriggerExit(Collider other)
    {
        if(other.gameObject == nearObject)
        {
            nearObject = null;
        }
    }
    public GameObject GetNearObject()
    {
        return nearObject;
    }
}