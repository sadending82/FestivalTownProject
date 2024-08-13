using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneManagerEx
{
    /// <summary>
    /// �׳� �� ��ȯ �Ŵ�����.
    /// Ex ���� ������ �̹� ����Ƽ���� SceneManager�� �־...
    /// </summary>

    public BaseScene CurrentScene { get { return GameObject.FindObjectOfType<BaseScene>(); } }

    string GetSceneName(Define.Scene type)
    {
        string name = System.Enum.GetName(typeof(Define.Scene), type); // Reflection �Դϴ�
        return name;
    }

    public void LoadScene(Define.Scene type)
    {
        Managers.Clear();

        SceneManager.LoadScene(GetSceneName(type));
    }

    public void Clear()
    {
        CurrentScene.Clear();
    }
}
