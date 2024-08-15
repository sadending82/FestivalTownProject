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
    public GameObject bombPrefab;
    public float bombLifeTime = 5;   // 초단위
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
            tempBomb.GetComponent<Bomb>().SetId(i);
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
        // 단위 맞춰주기
        x *= -2;
        y *= -2;
        GameObject reusedBomb = null;
        // 미리 생성되어 있는 폭탄부터 사용
        for (int i = 0; i < bombs.Count; ++i)
        {
            if (bombs[i].activeSelf == false)
            {
                reusedBomb = bombs[i];
                break;
            }
        }
        // 사용 가능한 폭탄이 없으면 새로 생성
        if (reusedBomb == null)
        {
            GameObject newBomb = Instantiate(bombPrefab) as GameObject;

            newBomb.transform.parent = transform;
            newBomb.GetComponent<Bomb>().SetId(bombs.Count);
            bombs.Add(newBomb);
            reusedBomb = newBomb;
        }

        reusedBomb.SetActive(true);
        // 폭탄 위치 설정
        reusedBomb.gameObject.GetComponent<Bomb>().SetLifeTime(lifeTime);
        reusedBomb.gameObject.transform.position = new Vector3(x + offsetX, createHeight, y + offsetY);
        reusedBomb.gameObject.SetActive(true);
    }
}
