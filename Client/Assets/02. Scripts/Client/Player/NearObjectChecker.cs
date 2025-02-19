using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NearObjectChecker : MonoBehaviour
{
    public GameObject nearObject;
    public GameObject guideUI;
    public CharacterStatus CharacterStatus;

    private void Awake()
    {
        nearObject = null;
    }

    private void FixedUpdate()
    {
        // 가까이 있다고 생각했던 오브젝트가 사라질 때
        if (nearObject != null && nearObject.activeSelf == false)
        {
            nearObject = null;
            UpdateUI();
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
                        // other이 현재 nearObject보다 가깝다면 교체
                        if (Vector3.Distance(this.transform.position, other.transform.position) < Vector3.Distance(this.transform.position, nearObject.transform.position))
                        {
                            nearObject = other.gameObject;
                        }
                    }
                }
            }
            else if (other.tag == "Weapon")
            {
                Weapon targetWeapon = other.GetComponent<Weapon>();
                if (targetWeapon.GetIsPickUp() == false)
                {
                    if (nearObject == null)
                    {
                        nearObject = other.gameObject;
                    }
                    else
                    {
                        // other이 현재 nearObject보다 가깝다면 교체
                        if (Vector3.Distance(this.transform.position, other.transform.position) < Vector3.Distance(this.transform.position, nearObject.transform.position))
                        {
                            nearObject = other.gameObject;
                        }
                    }
                }
            }
            UpdateUI();
        }
    }
    private void OnTriggerExit(Collider other)
    {
        if (other.gameObject == nearObject)
        {
            nearObject = null;
        }
    }
    public GameObject GetNearObject()
    {
        return nearObject;
    }
    private void UpdateUI()
    {
        if (CharacterStatus.GetAmIPlayer() == false)
        {
            return;
        }
        if (guideUI.transform.GetChild(2).gameObject.activeSelf == true ||
        guideUI.transform.GetChild(3).GetChild(0).GetChild(0).gameObject.activeSelf == true)
        {
            return;
        }
        guideUI.transform.GetChild(0).gameObject.SetActive(false);
        guideUI.transform.GetChild(1).gameObject.SetActive(false);
        if (nearObject != null && nearObject.tag == "Weapon"
            && CharacterStatus.GetIsHaveWeapon() == false)
        {
            guideUI.transform.GetChild(0).gameObject.SetActive(true);
        }
        else if (nearObject != null && nearObject.tag == "Bomb"
            && CharacterStatus.GetIsHaveBomb() == false)
        {
            guideUI.transform.GetChild(1).gameObject.SetActive(true);
        }
    }
}
