using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponObjectManager : MonoBehaviour
{
    private float offsetX = 20;
    private float offsetY = 10;
    private float createHeight = 8;

    public int initialWeapons = 10;

    [SerializeField]
    private GameObject weapons;

    public void Init()
    {
        weapons = GameObject.Find("@Weapons");
        if(weapons == null)
        {
            weapons = new GameObject { name = "@Weapons" };
        }
    }
    public void SpawnWeapon(float x, float y, int id)
    {
        // 단위 맞춰주기
        x *= -1;
        y *= -1;
        GameObject reusedWeapon = null;

        reusedWeapon = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Weapon"), weapons.transform).gameObject;

        reusedWeapon.gameObject.SetActive(true);
        reusedWeapon.gameObject.GetComponent<Weapon>().SetId(id);
        reusedWeapon.gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
    }
    public GameObject FindWeaponById(int targetId)
    {
        int numOfChild = weapons.transform.childCount;
        for (int i = 0; i < numOfChild; ++i)
        {
            if (weapons.transform.GetChild(i).GetComponent<Weapon>().GetId() == targetId)
            {
                return weapons.transform.GetChild(i).gameObject;
            }
        }

        Debug.Log("ERROR!!! : Can't Find Target Weapon By Id : " + targetId);
        return null;
    }
}
