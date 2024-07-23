using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{

    public static NetworkManager instance;

    void Awake()
    {
        DontDestroyOnLoad(gameObject);

        if(instance == null)
        {

        }
        else if (instance != this)
        {
            Destroy(gameObject);
        }

    }

}
