using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public abstract class BaseScene : MonoBehaviour
{
    public Define.Scene SceneType { get; protected set; } = Define.Scene.None;

    private void Awake()
    {
        Init();
    }

    protected virtual void Init()
    {
        Object obj = GameObject.FindObjectOfType(typeof(EventSystem));
        if (obj == null)
        {
            // 나중에 UI 만들려면 event system이 있어야 함
            // 근데 없으면 만들어 줘야지
            // 이거도 귀찮으니까 Resources에 넣어놓고 프리팹으로 불러버리자
            Managers.Resource.Instantiate("UI/EventSystem").name = "@EventSystem";
        }
    }

    //Clear는 그냥 다른애들 한테 맡겨버리자
    public abstract void Clear();
}
