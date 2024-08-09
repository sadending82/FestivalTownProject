using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CubeObjectManager : MonoBehaviour
{
    private float offsetX = 19;
    private float offsetY = 9;
    private float creatHeight = 8;

    public static CubeObjectManager instance;
    public int initialCubes = 10;
    public GameObject cubePrefab;
    public int cubeTypes = 2;
    private List<GameObject> cubes = new List<GameObject>();

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
        for(int i=0;i<initialCubes;++i)
        {
            GameObject tempCube = Instantiate(cubePrefab) as GameObject;

            tempCube.transform.parent = transform;
            tempCube.SetActive(false);
            cubes.Add(tempCube);
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
        GameObject reusedCube = null;
        // ���� �����ֱ�
        x *= -2;
        y *= -2;
        // �̸� �����Ǿ� �ִ� ť����� ���
        for(int i=0;i<cubes.Count;++i)
        {
            if (cubes[i].activeSelf == false)
            {
                reusedCube = cubes[i];
                break;
            }
        }
        // ��� ������ ť�갡 ������ ���� ����
        if(reusedCube == null)
        {
            GameObject newCube = Instantiate(cubePrefab) as GameObject;

            newCube.transform.parent = transform;
            cubes.Add(newCube);
            reusedCube = newCube;
        }
        
        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedCube.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedCube.gameObject.transform.GetChild(type).gameObject.SetActive(true);
        reusedCube.gameObject.transform.GetChild(type).gameObject.transform.position = new Vector3(x + offsetX, creatHeight, y + offsetY);
    }
}