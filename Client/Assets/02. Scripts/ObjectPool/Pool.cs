using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

/// <summary>
/// 실제 오브젝트 풀 클래스.
/// /// </summary>
class Pool
{
    public GameObject Original { get; private set; } // 원본 게임오브젝트.
    public Transform Root { get; set; } // pool의 최상위 오브젝트.

    Stack<Poolable> _poolStack = new Stack<Poolable>();

    // 일단은 5개 정도로 테스트를 해봅시따
    public void Init(GameObject origin, int count = 5)
    {
        Original = origin;

        Root = new GameObject().transform;
        Root.name = $"{origin.name}_Root"; // 루트 이름은 그냥 오브젝트의 이름으로 지정

        for (int i = 0; i < count; i++)
        {
            Push(Create());
        }
    }

    Poolable Create()
    {
        GameObject go = Object.Instantiate<GameObject>(Original);
        go.name = Original.name; // 그냥 뒤에 (clone) 붙는거 지우려고 하는거
        return go.GetOrAddcomponent<Poolable>();
    }

    /// <summary>
    /// 오브젝트 풀에 넣어주는 함수 ( 오브젝트 그냥 OFF 하는거 )
    /// </summary>
    /// <param name="poolable">넣을 컴포넌트</param>
    public void Push(Poolable poolable) 
    {
        if (poolable == null) return;

        poolable.transform.parent = Root;
        poolable.gameObject.SetActive(false);
        poolable.isUsing = false;

        _poolStack.Push(poolable);
    }

    /// <summary>
    /// 오브젝트 풀로부터 꺼내오는 함수 (오브젝트 ON!)
    /// </summary>
    /// <param name="parent">꺼낸 오브젝트는 어디다가 넣을거에요?</param>
    /// <returns></returns>
    public Poolable Pop(Transform parent)
    {
        // TODO: 앞으로, 개발이 진행되면서 DontDestroyOnLoad의 특성으로 인해 문제가 생길 가능성이 있으므로
        // 나중에 Scene 관리를 해주면서 이 부분도 수정해 주어야 할것으로 보입니다

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

        // 그냥 null로 설정하면 DontDestroyOnLoad가 풀리지 않아서 문제가 생길 수 있으므로
        // 일단 Scene의 자식으로 설정한 뒤에 다시 풀어 주면 괜찮지 않을까?
        if(parent == null)
        {
            data.transform.parent = Managers.Scene.CurrentScene.transform;
        }

        data.transform.parent = parent;
        data.isUsing = true;

        return data;
    }
}
