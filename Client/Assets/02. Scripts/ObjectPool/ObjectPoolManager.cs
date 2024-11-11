using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectPoolManager
{
    Dictionary<string, Pool> _pool = new Dictionary<string, Pool>();
    Transform _root;


    public void Init()
    {
        if(_root == null)
        {
            _root = new GameObject { name = "@Pool_Root" }.transform;
            Object.DontDestroyOnLoad(_root);
        }
    }

    public void Push(Poolable poolObj)
    {
        string name = poolObj.gameObject.name;
        if (!_pool.ContainsKey(name))
        {
            GameObject.Destroy(poolObj.gameObject); // 나중에 Destroy라는 함수가 겹쳐질까봐 혹시 몰라서 붙여놓음
            return;
        }

        _pool[name].Push(poolObj);
    }

    /// <summary>
    /// Pool을 만들어 주는 함수
    /// </summary>
    /// <param name="origin">원본이 뭐임?</param>
    /// <param name="count">미리 몇개 만들어 둘거임</param>
    public void CreatePool(GameObject origin, int count = 5)
    {
        Pool pool = new Pool();
        pool.Init(origin, count);
        pool.Root.parent = _root;

        _pool.Add(origin.name, pool);
    }

    /// <summary>
    /// 오브젝트 빼와!
    /// </summary>
    /// <param name="origin">원본은 뭐에요?</param>
    /// <param name="parent">꺼낸 오브젝트는 어디다 넣을거에요?</param>
    /// <returns></returns>
    public Poolable Pop(GameObject origin, Transform parent = null)
    {
        if (_pool != null) return null;
        
        if (!_pool.ContainsKey(origin.name))
        {
            CreatePool(origin);
        }

        return _pool[origin.name].Pop(parent);
    }

    /// <summary>
    /// 특정 이름을 가진 풀로 부터 원본 게임오브젝트를 가져오는 함수
    /// </summary>
    /// <param name="name">게임 오브젝트의 이름</param>
    /// <returns></returns>
    public GameObject GetOrigin(string name)
    {
        if (!_pool.ContainsKey(name))
        {
            return null;
        }

        return _pool[name].Original;
    }

    public void Clear()
    {
        foreach (Transform child in _root)
        {
            GameObject.Destroy(child.gameObject);
        }

        _pool.Clear();
    }
}
