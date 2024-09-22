using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BombObjectManager : MonoBehaviour
{
    private float createHeight = 8;

    public static BombObjectManager instance;
    public int initialBombs = 10;

    [SerializeField]
    private GameObject bombs;
    
    public void Init()
    {
        bombs = GameObject.Find("@Bombs");
        if (bombs == null)
        {
            bombs = new GameObject { name = "@Bombs" };
        }
    }

    public void SpawnBomb(float x, float z, int id)
    {
        if (Managers.BombObject.FindBombById(id) != null)
        {
            return;
        }

        // 단위 맞춰주기
        x *= -1;
        z *= -1;
        GameObject reusedBomb = null;

        reusedBomb = Managers.ObjectPool.Pop(Managers.ObjectPool.GetOrigin("Bomb"), bombs.transform).gameObject;

        reusedBomb.gameObject.SetActive(true);
        reusedBomb.gameObject.GetComponent<Bomb>().SetId(id);
        reusedBomb.gameObject.transform.position = new Vector3(x, createHeight, z);

        Managers.Sound.Play3D("Sfx_Bomb_Fuse", reusedBomb, Define.Sound.Bgm);
    }

    public GameObject FindBombById(int targetBombId)
    {
        int numOfChild = bombs.transform.childCount;
        for (int i = 0; i < numOfChild; ++i)
        {
            if (bombs.transform.GetChild(i).GetComponent<Bomb>().GetId() == targetBombId)
            {
                return bombs.transform.GetChild(i).gameObject;
            }
        }

        Debug.Log("ERROR!!! : Can't Find Target Bomb By Id : " + targetBombId);
        return null;
    }
}
