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
    public void SpawnBomb(int x, int y, int id)
    {
        // ���� �����ֱ�
        x *= -2;
        y *= -2;
        GameObject reusedBomb = null;

        reusedBomb = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Bomb"), transform).gameObject;

        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedBomb.gameObject.SetActive(true);
        // ������Ʈ Ǯ���� ���ö� ���̵� �־���
        reusedBomb.gameObject.GetComponent<Bomb>().SetId(id);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedBomb.gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
        reusedBomb.gameObject.SetActive(true);
    }

    public GameObject FindBombById(int targetBombId)
    {
        int numOfChild = this.transform.childCount;
        for (int i = 0; i < numOfChild; ++i)
        {
            if (this.transform.GetChild(i).GetComponent<Bomb>().GetId() == targetBombId)
            {
                return transform.GetChild(i).gameObject;
            }
        }

        Debug.Log("ERROR!!! : Can't Find Target Bomb By Id : " + targetBombId);
        return null;
    }
}
