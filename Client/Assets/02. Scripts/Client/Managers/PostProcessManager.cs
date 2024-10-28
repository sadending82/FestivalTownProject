using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;

public class PostProcessManager
{
    PostProcessVolume PostProcesser;

    public void Init()
    {
        PostProcesser = null;
    }

    /// <summary>
    /// 포스트 프로세스의 대상을 설정해주는 함수
    /// </summary>
    public void SetPostProcesser()
    {
        GameObject root = GameObject.Find("@PostProcess");
        if (root == null) return;
        PostProcesser = root.GetComponent<PostProcessVolume>();
    }

    public void SetScreenGray()
    {
        if(PostProcesser == null) return;

        ColorGrading cg;

        if(PostProcesser.profile.TryGetSettings(out cg))
        {
            cg.saturation.Override(-100.0f);
        }
    }

    public void SetScreenNormal()
    {
        if (PostProcesser == null) return;

        ColorGrading cg;

        if (PostProcesser.profile.TryGetSettings(out cg))
        {
            cg.saturation.Override(0.0f);
        }
    }

    public void Clear()
    {
        PostProcesser = null;
    }

}
