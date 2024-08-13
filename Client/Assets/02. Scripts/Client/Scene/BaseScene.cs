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
            // ���߿� UI ������� event system�� �־�� ��
            // �ٵ� ������ ����� �����
            // �̰ŵ� �������ϱ� Resources�� �־���� ���������� �ҷ�������
            Managers.Resource.Instantiate("UI/EventSystem").name = "@EventSystem";
        }
    }

    //Clear�� �׳� �ٸ��ֵ� ���� �ðܹ�����
    public abstract void Clear();
}
