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
            GameObject.Destroy(poolObj.gameObject); // ���߿� Destroy��� �Լ��� ��������� Ȥ�� ���� �ٿ�����
            return;
        }

        _pool[name].Push(poolObj);
    }

    /// <summary>
    /// Pool�� ����� �ִ� �Լ�
    /// </summary>
    /// <param name="origin">������ ����?</param>
    /// <param name="count">�̸� � ����� �Ѱ���</param>
    public void CreatePool(GameObject origin, int count = 5)
    {
        Pool pool = new Pool();
        pool.Init(origin, count);
        pool.Root.parent = _root;

        _pool.Add(origin.name, pool);
    }

    /// <summary>
    /// ������Ʈ ����!
    /// </summary>
    /// <param name="origin">������ ������?</param>
    /// <param name="parent">���� ������Ʈ�� ���� �����ſ���?</param>
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
    /// Ư�� �̸��� ���� Ǯ�� ���� ���� ���ӿ�����Ʈ�� �������� �Լ�
    /// </summary>
    /// <param name="name">���� ������Ʈ�� �̸�</param>
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
