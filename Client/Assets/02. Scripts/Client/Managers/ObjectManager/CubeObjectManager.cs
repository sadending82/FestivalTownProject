using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;
using System;

public class CubeObjectManager : MonoBehaviour
{
    private const float CREATE_HEIGHT = 10;

    public int initialCubes = 10;
    public int cubeTypes = 2;

    [SerializeField]
    private GameObject cubes;

    public void Init()
    {
        cubes = GameObject.Find("@Cubes");
        if (cubes == null)
        {
            cubes = new GameObject { name = "@Cubes" };
        }
    }

    private void Update()
    {
    }

    public void SpawnCube(int x, int z, eBlockType type)
    {
        // 맵의 칸 별로의 높이를 저장
        float targetHeight = Managers.Map.GetMapHeight(x, z);
        int iType = 0;
        switch(type)
        {
            case eBlockType.BT_BLOCK_2_2_1:
                {
                    Managers.Map.AddMapHeight(x, z, 1.0f);
                    iType = 0;
                }
                break;
            case eBlockType.BT_BLOCK_2_2_2:
                {
                    Managers.Map.AddMapHeight(x, z, 2.0f);
                    iType = 1;
                }
                break;
            default:
                {
                    Debug.Log("ERROR!! : SpawnCube(), Type is Wrong !!!");
                }
                break;
        }
        GameObject reusedCube = null;
        // 단위 맞춰주기
        x *= 2;
        z *= 2;

        try
        {
            reusedCube = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Cube"), cubes.transform).gameObject;
        }           
        catch (NullReferenceException)
        {
            return;
        }


        // 큐브 생성 위치와 큐브 타입 설정, 플레이어를 밀어내는 Pusher 작동
        reusedCube.SetActive(true);
        // 타입에 해당하는 큐브 활성화, 포지션 설정
        reusedCube.gameObject.transform.GetChild(iType).gameObject.SetActive(true);
        reusedCube.gameObject.transform.GetChild(iType).gameObject.GetComponent<Cube>().SetTargetHeight(targetHeight);
        reusedCube.gameObject.transform.GetChild(iType).gameObject.transform.position = new Vector3(x + 1, targetHeight + CREATE_HEIGHT, z + 1);
        int rand = UnityEngine.Random.Range(0, 3);
        reusedCube.gameObject.transform.GetChild(iType).gameObject.transform.rotation = Quaternion.Euler(0, rand * 90.0f, 0);

        Managers.Sound.Play3D("Sfx_Block_Falling", reusedCube);
    }

    public void Clear()
    {
        cubes.SetActive(false);
    }
}