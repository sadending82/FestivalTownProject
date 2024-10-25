using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_WorldSpace : UI_Base
{
    int _key;

    public void SetKey(int key)
    {
        _key = key;
    }

    public int GetKey()
    {
        return _key;
    }

    public override void Init()
    {
        
    }
}
