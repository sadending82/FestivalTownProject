using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class CheatManager
{
    bool isEnable = false;

    public void Init()
    {

        var filePath = string.Format("{0}/Cheat/Cheat.config", Application.persistentDataPath);

        if (File.Exists(filePath))
        {
            isEnable = true;
        }

    }

    public bool IsEnable()
    {
        return isEnable; 
    }
}
