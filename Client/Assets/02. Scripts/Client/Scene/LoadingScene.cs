using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoadingScene : BaseScene
{
    bool LoadStart = false;
    bool NextSceneActivate = false;

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
        AsyncOp.allowSceneActivation = false;

        Debug.Log("Stuff Start.");

        float loadTime = 0.0f;
        while (loadTime < 0.5f)
        {
            yield return null;

            //unscaled는 TimeScale에 따라 달라지지 않는 DeltaTime임.
            loadTime += Time.unscaledDeltaTime;
        }

        Debug.Log("loadTime End");

        Managers.Network.GetPacketManager().SendGameReady();

        while (!NextSceneActivate)
        {
            yield return null;
            if (NextSceneActivate)
            {
                AsyncOp.allowSceneActivation = true;
                break;
            }
            
        }
    }

    public void ChangeNextScene()
    {
        NextSceneActivate = true;
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
