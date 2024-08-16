using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CubeObjectManager : MonoBehaviour
{
    private float offsetX = 19;
    private float offsetY = 9;
    private float createHeight = 8;

    public static CubeObjectManager instance;
    public int initialCubes = 10;
    public int cubeTypes = 2;
    private List<GameObject> cubes = new List<GameObject>();

    private float[,] aMapHeight = new float[30, 30];

    // ------ Test -------
    public int testXY = 0;

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }

        MakeCubes();
    }

    private void MakeCubes()
    {
        List<GameObject> list = new List<GameObject>();

        for(int i=0;i<initialCubes;++i)
        {
            list.Add(Managers.Resource.Instantiate("Cube"));
        }

        foreach(GameObject obj in list)
        {
            Managers.Resource.Destroy(obj);
        }
    }
    private void Update()
    {
        if(Input.GetKeyUp(KeyCode.R))
        {
            SpawnCube(testXY, testXY, testXY % 2);
            testXY++;
            testXY %= 10;
        }
    }

    public void SpawnCube(int x, int y, int type = 1)
    {
        // ���� ĭ ������ ���̸� ����
        float targetHeight = aMapHeight[x, y];
        switch(type)
        {
            case 0:
                {
                    targetHeight += 0.5f;
                    aMapHeight[x, y] += 1f;
                }
                break;
            case 1:
                {
                    targetHeight += 1f;
                    aMapHeight[x, y] += 2f;
                }
                break;
            default:
                {
                    Debug.Log("ERROR!! : SpawnCube(), Type is Wrong !!!");
                }
                break;
        }
        GameObject reusedCube = null;
        // ���� �����ֱ�
        x *= -2;
        y *= -2;

        //// �̸� �����Ǿ� �ִ� ť����� ���
        //for(int i=0;i<cubes.Count;++i)
        //{
        //    if (cubes[i].activeSelf == false)
        //    {
        //        reusedCube = cubes[i];
        //        break;
        //    }
        //}
        //// ��� ������ ť�갡 ������ ���� ����
        //if(reusedCube == null)
        //{
        //    GameObject newCube = Instantiate(cubePrefab) as GameObject;

        //    newCube.transform.parent = transform;
        //    cubes.Add(newCube);
        //    reusedCube = newCube;
        //}

        reusedCube = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Cube"), transform).gameObject;

        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedCube.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedCube.gameObject.transform.GetChild(type).gameObject.SetActive(true);
        reusedCube.gameObject.transform.GetChild(type).gameObject.GetComponent<Cube>().SetTargetHeight(targetHeight);
        reusedCube.gameObject.transform.GetChild(type).gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
    }
}