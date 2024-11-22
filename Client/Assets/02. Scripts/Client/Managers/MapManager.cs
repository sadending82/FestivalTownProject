using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using System.IO;

public class MapManager : MonoBehaviour
{
    [SerializeField] private GameObject map;
    [SerializeField] private int mapIndex;
    [SerializeField] private int mapThema;
    [SerializeField] private MapCode mapCode;
    private float[,] mapHeight;

    public void Init()
    {
        map = GameObject.Find("@Map");
        if(map == null )
        {
            map = new GameObject { name = "@Map" };
        }
    }

    public void LoadGameMap(int mapIndex = 11201, int mapThema = 1, MapCode mapCode = MapCode.Map_FITH_1vs1)
    {
        StreamReader reader = new StreamReader(Application.dataPath + $"/11. GameData/Map/{mapIndex}.txt");
        string readLine;

        //맵 사이즈
        readLine = reader.ReadLine();
        string[] sMapSize = readLine.Split(' ');
        int mapSizeX, mapSizeZ;
        int.TryParse(sMapSize[0], out mapSizeX);
        int.TryParse(sMapSize[1], out mapSizeZ);
        mapHeight = new float[mapSizeX, mapSizeZ];

        //동상 정보
        for (int i = 0; i < 3; ++i)
        {
            readLine = reader.ReadLine();
            string[] sStatue = readLine.Split(' ');
            int teamNumber;
            Vector3 position;
            int directionNum;
            int.TryParse(sStatue[0], out teamNumber);
            float.TryParse(sStatue[1], out position.x);
            float.TryParse(sStatue[2], out position.y);
            float.TryParse(sStatue[3], out position.z);
            int.TryParse(sStatue[4], out directionNum);
            Vector3 direction = new Vector3(0.0f, directionNum * 90.0f, 0.0f);
            LoadStatue(teamNumber, position, direction);
        }

        //맵 높이 정보
        for (int i = mapSizeZ - 1; i >= 0; --i)
        {
            readLine = reader.ReadLine();
            string[] sHeight = readLine.Split(" ");
            for (int j = 0; j < mapSizeX; ++j)
            {
                float height;
                float.TryParse(sHeight[j], out height);
                mapHeight[j, i] = height;
            }
        }

        //위 까지 잘 읽었는지 확인해줌
        readLine = reader.ReadLine();
        if(readLine != "MapStructure")
        {
            Debug.Log("ERROR!!! LoadGameMap(): Map Load ERROR!!!");
        }

        for (int i = mapSizeZ - 1; i >= 0; --i)
        {
            readLine = reader.ReadLine();
            string[] sCubeData = readLine.Split(" ");
            for (int j = 0; j < mapSizeX; ++j)
            {
                switch(sCubeData[j])
                {
                    //골대
                    case "a":
                        {
                        }
                        break;
                    case "b":
                        {
                        }
                        break;
                    case "c":
                        {
                        }
                        break;

                    //스폰지역
                    case "0":
                        {
                        }
                        break;
                    case "1":
                        {
                        }
                        break;
                    case "2":
                        {
                        }
                        break;

                    //큐브
                    case "n":
                        {
                        }
                        break;
                    case "o":
                        {
                        }
                        break;
                    case "x":
                        {
                        }
                        break;

                    default:
                        {
                            Debug.Log("ERROR!!! LoadGameMap(): Wrong MapData!!!");
                        }
                        break;
                }
            }
        }
    }

    public void LoadStatue(int teamNumber, Vector3 position, Vector3 direction)
    {
        GameObject Statue = Managers.Resource.Instantiate("Statue");
        Statue.transform.position = position;
        Statue.transform.rotation = Quaternion.Euler(direction);
        Statue.GetComponent<Statue>().SetTeam(teamNumber);
    }
    public float GetMapHeight(int x, int z)
    {
        if (x < 0 || z < 0)
        {
            return -1.0f;
        }
        return mapHeight[x, z];
    }

    public void Clear()
    {
        mapHeight = null;
    }
}
