using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using System.IO;
using TMPro;

public class MapManager : MonoBehaviour
{
    [SerializeField] private GameObject map;
    [SerializeField] private int mapIndex;
    [SerializeField] private int mapTheme;
    private float[,] mapHeight;
    private const float OFFSET_X = 1.0f;
    private const float OFFSET_Y = -2.0f;
    private const float OFFSET_Z = 1.0f;
    private const float GOALPOST_OFFSET_Y = OFFSET_Y - 1.0f;
    private Vector3[] spawnDirection = new Vector3[3];
    public int mapSizeX, mapSizeZ;

    // OFFSETS
    private const float MAP_THEME1_OFFSET_X = -81.3f;
    private const float MAP_THEME1_OFFSET_Y = -59.0f;
    private const float MAP_THEME1_OFFSET_Z = 40.0f;
    private const float MAP_THEME2_OFFSET_X = -81.3f;
    private const float MAP_THEME2_OFFSET_Y = -59.0f;
    private const float MAP_THEME2_OFFSET_Z = 40.0f;

    public void Init()
    {
        map = GameObject.Find("@Map");
        if(map == null )
        {
            map = new GameObject { name = "@Map" };
        }
    }

    public void LoadGameMap(int mapIndex, int mapTheme = 1)
    {
        StreamReader reader = new StreamReader(Application.streamingAssetsPath + $"/Map/{mapIndex}.txt");
        string readLine;

        //맵 사이즈
        readLine = reader.ReadLine();
        string[] sMapSize = readLine.Split(' ');
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

            if (teamNumber != -1)
            {
                Vector3 direction = new Vector3(0.0f, directionNum * 90.0f, 0.0f);
                spawnDirection[teamNumber] = direction;
                LoadStatue(teamNumber, position, direction);
            }
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

        string mapObjectPath = "MapObject/";
        switch(Managers.Game.mapTheme)
        {
            case 1:
                {
                    mapObjectPath += "Theme1/";
                }
                break;
            case 2:
                {
                    mapObjectPath += "Theme2/";
                }
                break;
            default:
                {
                    Debug.Log("ERROR!! LoadGameMap(): Wrong Map Theme !!!");
                }
                break;
        }
        for (int i = mapSizeZ - 1; i >= 0; --i)
        {
            readLine = reader.ReadLine();
            string[] sCubeData = readLine.Split(" ");
            for (int j = 0; j < mapSizeX; ++j)
            {
                float height = mapHeight[j, i];
                float tPosX = j * 2 + OFFSET_X;
                float tPosZ = i * 2 + OFFSET_Z;
                switch (sCubeData[j])
                {
                    //골대
                    case "a":
                        {
                            GameObject goalPost = Managers.Resource.Instantiate($"{mapObjectPath}GoalPost");
                            goalPost.GetComponent<GoalPost>().SetTeamNumber(0);
                            goalPost.transform.position = new Vector3(tPosX, height + GOALPOST_OFFSET_Y, tPosZ);
                            goalPost.transform.parent = map.transform;
                        }
                        break;
                    case "b":
                        {
                            GameObject goalPost = Managers.Resource.Instantiate($"{mapObjectPath}GoalPost");
                            goalPost.GetComponent<GoalPost>().SetTeamNumber(1);
                            goalPost.transform.position = new Vector3(tPosX, height + GOALPOST_OFFSET_Y, tPosZ);
                            goalPost.transform.parent = map.transform;
                        }
                        break;
                    case "c":
                        {
                            GameObject goalPost = Managers.Resource.Instantiate($"{mapObjectPath}GoalPost");
                            goalPost.GetComponent<GoalPost>().SetTeamNumber(2);
                            goalPost.transform.position = new Vector3(tPosX, height + GOALPOST_OFFSET_Y, tPosZ);
                            goalPost.transform.parent = map.transform;
                        }
                        break;

                    //스폰지역
                    case "0":
                        {
                            if (height > 0) FillCubeByHeight(j, i, height);
                            GameObject cube;
                            if ((int)height % 2 == 0)
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Red");
                            }
                            else
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_221_Red");
                                height += 1;
                            }
                            int rand = UnityEngine.Random.Range(0, 3);
                            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
                            cube.transform.position = new Vector3(tPosX, height + OFFSET_Y, tPosZ);
                            cube.transform.parent = map.transform;
                        }
                        break;
                    case "1":
                        {
                            if (height > 0) FillCubeByHeight(j, i, height);
                            GameObject cube;
                            if ((int)height % 2 == 0)
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Blue");
                            }
                            else
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_221_Blue");
                                height += 1;
                            }
                            int rand = UnityEngine.Random.Range(0, 3);
                            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
                            cube.transform.position = new Vector3(tPosX, height + OFFSET_Y, tPosZ);
                            cube.transform.parent = map.transform;
                        }
                        break;
                    case "2":
                        {
                            if (height > 0) FillCubeByHeight(j, i, height);
                            GameObject cube;
                            if ((int)height % 2 == 0)
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Green");
                            }
                            else
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_221_Green");
                                height += 1;
                            }
                            int rand = UnityEngine.Random.Range(0, 3);
                            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
                            cube.transform.position = new Vector3(tPosX, height + OFFSET_Y, tPosZ);
                            cube.transform.parent = map.transform;
                        }
                        break;

                    //큐브
                    case "n":
                        {
                            if (height > 0) FillCubeByHeight(j, i, height);
                            GameObject cube;
                            if ((int)height % 2 == 0)
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Basic");
                            }
                            else
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_221_Basic");
                                height += 1;
                            }
                            int rand = UnityEngine.Random.Range(0, 3);
                            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
                            cube.transform.position = new Vector3(tPosX, height + OFFSET_Y, tPosZ);
                            cube.transform.parent = map.transform;
                        }
                        break;
                    case "o":
                        {
                            if (height > 0) FillCubeByHeight(j, i, height);
                            GameObject cube;
                            if ((int)height % 2 == 0)
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Purple");
                            }
                            else
                            {
                                cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_221_Purple");
                                height += 1;
                            }
                            int rand = UnityEngine.Random.Range(0, 3);
                            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
                            cube.transform.position = new Vector3(tPosX, height + OFFSET_Y, tPosZ);
                            cube.transform.parent = map.transform;
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

        LoadMapTheme();
    }

    private void LoadMapTheme()
    {
        switch(Managers.Game.mapTheme)
        {
            case 1:
                {
                    GameObject mapTheme = Managers.Resource.Instantiate("MapTheme/MapTheme1");
                    mapTheme.transform.position = new Vector3(MAP_THEME1_OFFSET_X + (float)mapSizeX, MAP_THEME1_OFFSET_Y, MAP_THEME1_OFFSET_Z + (float)mapSizeZ);
                    mapTheme.transform.parent = map.transform;
                    RenderSettings.skybox = Resources.Load<Material>($"Prefabs/MapObject/Theme1/SkyBox");
                }
                break;
            case 2:
                {
                    GameObject mapTheme = Managers.Resource.Instantiate("MapTheme/MapTheme2");
                    mapTheme.transform.position = new Vector3(MAP_THEME2_OFFSET_X + (float)mapSizeX, MAP_THEME2_OFFSET_Y, MAP_THEME2_OFFSET_Z + (float)mapSizeZ);
                    mapTheme.transform.parent = map.transform;
                    RenderSettings.skybox = Resources.Load<Material>($"Prefabs/MapObject/Theme2/SkyBox");
                }
                break;
            default:
                {
                    Debug.Log("ERROR!! LoadMapTheme() : Wrong Map Theme!!!");
                }
                break;
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
    public void AddMapHeight(int x, int z, float extraHeight)
    {
        mapHeight[x, z] += extraHeight;
    }

    public void Clear()
    {
        mapHeight = null;
    }

    /// <summary>
    /// 높이 만큼 아래에 큐브를 채워주는 함수
    /// </summary>
    private void FillCubeByHeight(int x, int z, float height)
    {
        string mapObjectPath = "MapObject/";
        switch (Managers.Game.mapTheme)
        {
            case 1:
                {
                    mapObjectPath += "Theme1/";
                }
                break;
            case 2:
                {
                    mapObjectPath += "Theme2/";
                }
                break;
            default:
                {
                    Debug.Log("ERROR!! LoadGameMap(): Wrong Map Theme !!!");
                }
                break;
        }
        float fillPosX = x * 2 + OFFSET_X;
        float fillPosZ = z * 2 + OFFSET_Z;
        for (int i = 0; i <= (int)(height - 1) / 2; ++i)
        {
            GameObject cube = Managers.Resource.Instantiate($"{mapObjectPath}Cube_222_Basic");
            int rand = UnityEngine.Random.Range(0, 3);
            cube.transform.rotation = Quaternion.Euler(-90.0f, rand * 90.0f, 0);
            cube.transform.position = new Vector3(fillPosX, (i * 2) + OFFSET_Y, fillPosZ);
            cube.transform.parent = map.transform;
        }
    }
    public Vector3 GetSpawnDirectionByTeam(int teamNumber)
    {
        return spawnDirection[teamNumber];
    }
}
