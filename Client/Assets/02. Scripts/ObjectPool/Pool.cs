using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

/// <summary>
/// ���� ������Ʈ Ǯ Ŭ����.
/// /// </summary>
class Pool
{
    public GameObject Original { get; private set; } // ���� ���ӿ�����Ʈ.
    public Transform Root { get; set; } // pool�� �ֻ��� ������Ʈ.

    Stack<Poolable> _poolStack = new Stack<Poolable>();

    // �ϴ��� 5�� ������ �׽�Ʈ�� �غ��õ�
    public void Init(GameObject origin, int count = 5)
    {
        Original = origin;

        Root = new GameObject().transform;
        Root.name = $"{origin.name}_Root"; // ��Ʈ �̸��� �׳� ������Ʈ�� �̸����� ����

        for (int i = 0; i < count; i++)
        {
            Push(Create());
        }
    }

    Poolable Create()
    {
        GameObject go = Object.Instantiate<GameObject>(Original);
        go.name = Original.name; // �׳� �ڿ� (clone) �ٴ°� ������� �ϴ°�
        return go.GetOrAddcomponent<Poolable>();
    }

    /// <summary>
    /// ������Ʈ Ǯ�� �־��ִ� �Լ� ( ������Ʈ �׳� OFF �ϴ°� )
    /// </summary>
    /// <param name="poolable">���� ������Ʈ</param>
    public void Push(Poolable poolable) 
    {
        if (poolable == null) return;

        poolable.transform.parent = Root;
        poolable.gameObject.SetActive(false);
        poolable.isUsing = false;

        _poolStack.Push(poolable);
    }

    /// <summary>
    /// ������Ʈ Ǯ�κ��� �������� �Լ� (������Ʈ ON!)
    /// </summary>
    /// <param name="parent">���� ������Ʈ�� ���ٰ� �����ſ���?</param>
    /// <returns></returns>
    public Poolable Pop(Transform parent)
    {
        // TODO: ������, ������ ����Ǹ鼭 DontDestroyOnLoad�� Ư������ ���� ������ ���� ���ɼ��� �����Ƿ�
        // ���߿� Scene ������ ���ָ鼭 �� �κе� ������ �־�� �Ұ����� ���Դϴ�

        Poolable data;

        if(_poolStack.Count > 0)
        {
            data = _poolStack.Pop();
        }
        else
        {
            data = Create();
        }

        data.gameObject.SetActive(true);

        // �׳� null�� �����ϸ� DontDestroyOnLoad�� Ǯ���� �ʾƼ� ������ ���� �� �����Ƿ�
        // �ϴ� Scene�� �ڽ����� ������ �ڿ� �ٽ� Ǯ�� �ָ� ������ ������?
        if(parent == null)
        {
            data.transform.parent = Managers.Scene.CurrentScene.transform;
        }

        data.transform.parent = parent;
        data.isUsing = true;

        return data;
    }
}
