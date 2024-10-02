using PacketTable.GameTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TransparentObject : MonoBehaviour
{
    public bool IsTransParent { get; private set; } = false;

    private MeshRenderer[] meshRenderers;
    private WaitForSeconds delay = new WaitForSeconds(0.001f);
    private WaitForSeconds resetDelay = new WaitForSeconds(0.005f);
    
    private const float THRESHOLD_ALPHA = 0.25f;
    private const float THRESHOLD_MAX_TIMER = 0.5f;

    private bool isReseting = false;
    private float timer = 0f;
    private Coroutine timeCheckCoroutine;
    private Coroutine resetCoroutine;
    private Coroutine becomeTransparentCoroutine;

    private void Awake()
    {
        meshRenderers = GetComponentsInChildren<MeshRenderer>();
    }

    /// <summary>
    /// ��Ÿ�� �� RenderingMode�� �ٲ��ִ� �Լ�
    /// </summary>
    /// <param name="material"></param>
    /// <param name="mode"> 0 = Opaque, 1 = Cutout, 2 = Fade, 3 = Transparent </param>
    /// <param name="renderQueue"> ������ ������ �Ǵ� ������ ����,
    /// ������ : 2000, ��ī�̹ڽ� : 2500, ���� : 3000, ���̴��� ����ť ��� : -1</param>
    private void SetMaterialRenderingMode(Material material, float mode, int renderQueue)
    {
        material.SetFloat("_Mode", mode);
        //material.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.SrcColor);
        material.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.OneMinusSrcAlpha);
        material.SetInt("ZWrite", 0);
        material.DisableKeyword("_ALPHATEST_ON");
        material.EnableKeyword("_ALPHABLEND_ON");
        material.DisableKeyword("_ALPHAREMULTIPLY_ON");
        material.renderQueue = renderQueue;
    }
    // ����ȭ ���� �Լ�
    public void BecomeTransparent()
    {
        // �̹� �����ϸ�
        if(IsTransParent == true)
        {
            timer = 0;
            return;
        }
        
        // ���� ���̸�
        if(resetCoroutine != null && isReseting == true)
        {
            isReseting = false;
            IsTransParent = false;
            StopCoroutine(resetCoroutine);
        }

        SetMaterialTransparent();
        IsTransParent = true;
        becomeTransparentCoroutine = StartCoroutine(BecomeTransparentCoroutine());
    }
    private void SetMaterialTransparent()
    {
        for (int i = 0; i < meshRenderers.Length; ++i)
        {
            foreach(Material material in meshRenderers[i].materials)
            {
                SetMaterialRenderingMode(material, 3f, 3000);
            }
        }
    }
    private void SetMaterialOpaque()
    {
        for (int i = 0; i < meshRenderers.Length; ++i)
        {
            foreach (Material material in meshRenderers[i].materials)
            {
                SetMaterialRenderingMode(material, 0f, -1);
            }
        }
    }

    public void ResetOriginalTransparent()
    {
        SetMaterialOpaque();
        resetCoroutine = StartCoroutine(ResetOriginalTransparentCoroutine());
    }
    private IEnumerator BecomeTransparentCoroutine()
    {
        while (true)
        {
            bool isComplete = true;

            for (int i = 0; i < meshRenderers.Length; ++i)
            {
                if (meshRenderers[i].material.color.a > THRESHOLD_ALPHA)
                {
                    isComplete = false;
                }

                Color color = meshRenderers[i].material.color;
                color.a -= Time.deltaTime;
                meshRenderers[i].material.color = color;
            }

            if(isComplete == true)
            {
                CheckTimer();
                break;
            }

            yield return delay;
        }
    }
    private IEnumerator ResetOriginalTransparentCoroutine()
    {
        IsTransParent = false;

        while (true)
        {
            bool isComplete = true;

            for (int i = 0; i < meshRenderers.Length; ++i)
            {
                if (meshRenderers[i].material.color.a < 1f)
                {
                    isComplete = false;
                }

                Color color = meshRenderers[i].material.color;
                color.a += Time.deltaTime;
                meshRenderers[i].material.color = color;
            }

            if (isComplete == true)
            {
                isReseting = false;
                break;
            }

            yield return resetDelay;
        }
    }
    public void CheckTimer()
    {
        if (timeCheckCoroutine != null)
        {
            StopCoroutine(timeCheckCoroutine);
        }
        timeCheckCoroutine = StartCoroutine(CheckTimerCouroutine());
    }
    // ����ȭ ���¿��� �����ð�(THRESHOLD_MAX_TIMER) ���� ���̿� �������� ����ȭ�� Ǯ��
    private IEnumerator CheckTimerCouroutine()
    {
        timer = 0f;

        while (true)
        {
            timer += Time.deltaTime;

            if(timer > THRESHOLD_MAX_TIMER)
            {
                isReseting = true;
                ResetOriginalTransparent();
                break;
            }

            yield return null;
        }
    }
}
