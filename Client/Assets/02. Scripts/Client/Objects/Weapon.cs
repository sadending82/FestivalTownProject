using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Weapon : MonoBehaviour
{
    // ------ Server -------
    [SerializeField]
    private int id;

    private void Start()
    {
    }
    public void SetId(int id)
    {
        this.id = id;
    }
    public int GetId()
    {
        return id;
    }
}
