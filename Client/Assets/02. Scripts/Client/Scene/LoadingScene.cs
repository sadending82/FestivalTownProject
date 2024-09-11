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
        //리플렉션 그대로 가지고 옴
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
            //Awake나 Start에서 실행하면 LoadAsync가 무시되는 것이 발견되어 이쪽으로 이동시켜 보았음.
            StartCoroutine(LoadSceneAsyncStuff(Define.Scene.Game));
            LoadStart = true;
        }
    }

    public override void Clear()
    {

    }
}
