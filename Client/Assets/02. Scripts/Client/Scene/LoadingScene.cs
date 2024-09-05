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
        //���÷��� �״�� ������ ��
        //SceneManager.LoadScene(System.Enum.GetName(typeof(Define.Scene), type));

        AsyncOperation AsyncOp = SceneManager.LoadSceneAsync(System.Enum.GetName(typeof(Define.Scene), type));
        AsyncOp.allowSceneActivation = false;

        Debug.Log("Stuff Start.");

        float loadTime = 0.0f;
        while (loadTime < 0.5f)
        {
            yield return null;

            //unscaled�� TimeScale�� ���� �޶����� �ʴ� DeltaTime��.
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
            //Awake�� Start���� �����ϸ� LoadAsync�� ���õǴ� ���� �߰ߵǾ� �������� �̵����� ������.
            StartCoroutine(LoadSceneAsyncStuff(Define.Scene.Game));
            LoadStart = true;
        }
    }

    public override void Clear()
    {

    }
}
