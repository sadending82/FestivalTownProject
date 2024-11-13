using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using System;

public class WeaponObjectManager : MonoBehaviour
{
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
    public void SpawnWeapon(float x, float z, eWeaponType type, int id)
    {
        if(Managers.WeaponObject.FindWeaponById(id) != null)
        {
            return;
        }

        GameObject reusedWeapon = null;

        try
        {
            switch (type)
            {
                case eWeaponType.WT_BAT:
                    {
                        reusedWeapon = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Bat"), weapons.transform).gameObject;
                    }
                    break;
                case eWeaponType.WT_FRYING_PAN:
                    {
                        reusedWeapon = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Frypan"), weapons.transform).gameObject;
                    }
                    break;
                default:
                    {
                        Debug.Log("Error !!! SpawnWeapon, Wrong Weapon Type !!!");
                    }
                    break;
            }
        }
        catch(NullReferenceException)
        {
            return;
        }   

        reusedWeapon.gameObject.SetActive(true);
        reusedWeapon.gameObject.GetComponent<Weapon>().SetId(id);
        reusedWeapon.gameObject.transform.position = new Vector3(x, createHeight, z);
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
        return null;
    }

    public void Clear()
    {
        weapons.SetActive(false);
    }
}
