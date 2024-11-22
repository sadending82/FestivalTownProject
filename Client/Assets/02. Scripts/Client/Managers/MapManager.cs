using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements.Experimental;
using NetworkProtocol;

public class MapManager : MonoBehaviour
{
    [SerializeField] private GameObject map;
    [SerializeField] private int mapIndex;
    [SerializeField] private int mapThema;
    [SerializeField] private MapCode mapCode;

    public void Init()
    {
        map = GameObject.Find("@Map");
        if(map == null )
        {
            map = new GameObject { name = "@Map" };
        }
    }

    public void LoadGameMap(int mapIndex, int mapThema = 1, MapCode mapCode = MapCode.Map_FITH_1vs1)
    {

    }
}
