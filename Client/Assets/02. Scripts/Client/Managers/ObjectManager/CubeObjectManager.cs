using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;

public class CubeObjectManager : MonoBehaviour
{
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

    public void SpawnCube(int x, int y, eBlockType type)
    {
        // ���� ĭ ������ ���̸� ����
        float targetHeight = aMapHeight[x, y];
        int iType = 0;
        switch(type)
        {
            case eBlockType.BT_BLOCK_2_2_1:
                {
                    targetHeight += 0.5f;
                    aMapHeight[x, y] += 1f;
                    iType = 0;
                }
                break;
            case eBlockType.BT_BLOCK_2_2_2:
                {
                    targetHeight += 1f;
                    aMapHeight[x, y] += 2f;
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
        // ���� �����ֱ�
        x *= -2;
        y *= -2;

        reusedCube = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Cube"), cubes.transform).gameObject;

        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedCube.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedCube.gameObject.transform.GetChild(iType).gameObject.SetActive(true);
        reusedCube.gameObject.transform.GetChild(iType).gameObject.GetComponent<Cube>().SetTargetHeight(targetHeight);
        reusedCube.gameObject.transform.GetChild(iType).gameObject.transform.position = new Vector3(x - 1, createHeight, y - 1);

        Managers.Sound.Play3D("Sfx_Block_Falling", reusedCube);
    }

    public void Clear()
    {
        for (int i = 0; i < 30; ++i)
        {
            for (int j = 0; j < 30; ++j)
            {
                aMapHeight[i, j] = 0f;
            }
        }
    }
}