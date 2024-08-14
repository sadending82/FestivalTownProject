using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BombObjectManager : MonoBehaviour
{
    private float createHeight = 8;

    public static BombObjectManager instance;
    public int initialBombs = 10;
    public GameObject bombPrefab;
    public float bombLifeTime = 5;   // �ʴ���
    private List<GameObject> bombs = new List<GameObject>();

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
        for (int i = 0; i < initialBombs; ++i)
        {
            GameObject tempBomb = Instantiate(bombPrefab) as GameObject;

            tempBomb.transform.parent = transform;
            tempBomb.SetActive(false);
            bombs.Add(tempBomb);
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
        GameObject reusedBomb = null;
        // �̸� �����Ǿ� �ִ� ť����� ���
        for (int i = 0; i < bombs.Count; ++i)
        {
            if (bombs[i].activeSelf == false)
            {
                reusedBomb = bombs[i];
                break;
            }
        }
        // ��� ������ ť�갡 ������ ���� ����
        if (reusedBomb == null)
        {
            GameObject newBomb = Instantiate(bombPrefab) as GameObject;

            newBomb.transform.parent = transform;
            bombs.Add(newBomb);
            reusedBomb = newBomb;
        }

        // ť�� ���� ��ġ�� ť�� Ÿ�� ����, �÷��̾ �о�� Pusher �۵�
        reusedBomb.SetActive(true);
        // Ÿ�Կ� �ش��ϴ� ť�� Ȱ��ȭ, ������ ����
        reusedBomb.gameObject.GetComponent<Bomb>().SetLifeTime(lifeTime);
        reusedBomb.gameObject.transform.position = new Vector3(x, createHeight, y);
        reusedBomb.gameObject.SetActive(true);
    }
}
