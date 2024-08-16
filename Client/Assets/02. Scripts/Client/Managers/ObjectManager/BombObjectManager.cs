using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BombObjectManager : MonoBehaviour
{
    private float offsetX = 19;
    private float offsetY = 9;
    private float createHeight = 8;

    public static BombObjectManager instance;
    public int initialBombs = 10;
    public float bombLifeTime = 5;   // �ʴ���

    public int bombIdCount = 0;

    // ------ Test -------
    public int testXY = 0;

    void Start()
    {
        if(instance == null)
        {
            instance = new BombObjectManager();
        }
        MakeBombs();
    }

    private void MakeBombs()
    {   
        List<GameObject> list = new List<GameObject>();

        for (int i = 0; i < initialBombs; ++i)
        {
            GameObject go = Managers.Resource.Instantiate("Bomb");
            list.Add(go);
        }

        foreach (GameObject obj in list)
        {
            Managers.Resource.Destroy(obj);

        }
    }
    void Update()
    {
        if (Input.GetKeyUp(KeyCode.T))
        {
            SpawnBomb((testXY * -2 + 19), (testXY * -2 + 9), bombLifeTime);
            testXY++;
            testXY %= 10;
        }
    }
    public void SpawnBomb(int x, int y, float lifeTime = 10)
    {
        // ���� �����ֱ�
        x *= -2;
        y *= -2;
        GameObject reusedBomb = null;

        reusedBomb = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Bomb"), transform).gameObject;

        // ������Ʈ Ǯ���� ���ö� ���̵� �־���
        reusedBomb.gameObject.GetComponent<Bomb>().SetId(bombIdCount++);
        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedBomb.gameObject.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedBomb.gameObject.GetComponent<Bomb>().SetLifeTime(lifeTime);
        reusedBomb.gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
        reusedBomb.gameObject.SetActive(true);
    }

    public GameObject FindBombById(int targetBombId)
    {
        for (int i = 0; i < transform.childCount; ++i)
        {
            if (transform.GetChild(i).GetComponent<Bomb>().GetId() == targetBombId)
            {
                return transform.GetChild(i).gameObject;
            }
        }
        
        Debug.Log("ERROR!!! : Can't Find Target Bomb By Id");
        return null;
    }
}
