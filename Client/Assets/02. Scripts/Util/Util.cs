using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class Util
{

    //
    // ���� Findchild�� �ΰ��� ���� ������ ������Ʈ�� ���� GameObject�� �� �ֱ� �����Դϴ�.
    // GetComponent�� ã�ƾ� �ϴµ� Component�� ������ �ȵ��ݾƿ�?
    //

    public static T FindChild<T>(GameObject go, string name = null, bool recursive = false) where T : UnityEngine.Object
    {
        if (go == null)
            return null;

        if (recursive == false)
        {
            for (int i = 0; i < go.transform.childCount; i++)
            {
                Transform transform = go.transform.GetChild(i);
                if (string.IsNullOrEmpty(name) || transform.name == name)
                {
                    T component = transform.GetComponent<T>();
                    if (component != null) return component;
                }
            }
        }
        else
        {
            foreach (T component in go.GetComponentsInChildren<T>())
            {
                if (string.IsNullOrEmpty(name) || component.name == name) return component;
            }
        }

        return null;
    }

    public static GameObject FindChild(GameObject go, string name = null, bool recursive = false)
    {
        Transform transform = FindChild<Transform>(go, name, recursive);
        if (transform == null) return null;

        return transform.gameObject;
    }

    public static T GetOrAddComponent<T>(GameObject go) where T : UnityEngine.Component
    {
        T component = go.GetComponent<T>();
        
        if(component == null) component = go.AddComponent<T>();

        return component;
    }

    public static bool IsEnglish(char ch)
    {
        if ((0x61 <= ch && ch <= 0x7A) || (0x41 <= ch && ch <= 0x5A)) return true;

        return false;
    }

    public static bool IsNumeric(char ch)
    {
        if (0x30 <= ch && ch <= 0x39) return true;

        return false;
    }

    public static bool IsKorean(char ch)
    {
        if ((0xAC00 <= ch && ch <= 0xD7A3) || (0x3131 <= ch && ch <= 0x318E)) return true;

        return false;
    }
}
