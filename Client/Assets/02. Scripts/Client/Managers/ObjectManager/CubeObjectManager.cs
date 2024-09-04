using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CubeObjectManager : MonoBehaviour
{
    private float offsetX = 19;
    private float offsetY = 9;
    private float createHeight = 8;

    public int initialCubes = 10;
    public int cubeTypes = 2;

    private float[,] aMapHeight = new float[30, 30];

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

        reusedCube = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Cube"), cubes.transform).gameObject;

        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedCube.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedCube.gameObject.transform.GetChild(type).gameObject.SetActive(true);
        reusedCube.gameObject.transform.GetChild(type).gameObject.GetComponent<Cube>().SetTargetHeight(targetHeight);
        reusedCube.gameObject.transform.GetChild(type).gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
    }
}