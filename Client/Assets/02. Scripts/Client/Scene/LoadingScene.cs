using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoadingScene : BaseScene
{
    bool LoadStart = false;

    protected override void Init()
    {
        base.Init();
        SceneType = Define.Scene.Loading;

        
    }

    IEnumerator LoadSceneAsyncStuff(Define.Scene type)
    {
        //���÷��� �״�� ������ ��
        //SceneManager.LoadScene(System.Enum.GetName(typeof(Define.Scene), type));

        AsyncOperation AsyncOp = SceneManager.LoadSceneAsync(System.Enum.GetName(typeof(Define.Scene), type));

        Debug.Log("Stuff Start.");

        while (!AsyncOp.isDone)
        {
            yield return null;
            Debug.Log($"Scene Load Progress {AsyncOp.progress * 100}%");
        }
    }

    private void Update()
    {

        if (!LoadStart)
        {
            //Awake�� Start���� �����ϸ� LoadAsync�� ���õǴ� ���� �߰ߵǾ� �������� �̵����� ������.
            StartCoroutine(LoadSceneAsyncStuff(Define.Scene.Game));
            LoadStart = true;
        }
    }

    public override void Clear()
    {

    }
}
