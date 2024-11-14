using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EffectManager : MonoBehaviour
{
    /// <summary>
    /// 이펙트를 플레이하는 함수
    /// </summary>
    /// <param name="effectName">
    /// BombExplosion, CubeFalldown, Ch_Groggy, Ch_Respawn, Victory
    /// </param>
    /// <param name="position"></param>
    public void PlayEffect(string effectName, Vector3 position)
    {
        ParticleSystem tEffect;
        switch (effectName)
        {
            case "BombExplosion":
                {
                    tEffect = Managers.Resource.Instantiate("Effect_BombExplosion").GetComponent<ParticleSystem>();
                    tEffect.transform.position = position;
                }
                break;
            case "CubeFalldown":
                {
                    tEffect = Managers.Resource.Instantiate("Effect_CubeFalldown").GetComponent<ParticleSystem>();
                    tEffect.transform.position = position;
                }
                break;
            case "Ch_Groggy":
                {
                    tEffect = Managers.Resource.Instantiate("Effect_Ch_Groggy").GetComponent<ParticleSystem>();
                    tEffect.transform.position = position;
                }
                break;
            case "Ch_Respawn":
                {
                    tEffect = Managers.Resource.Instantiate("Effect_Ch_Respawn").GetComponent<ParticleSystem>();
                    tEffect.transform.position = position;
                }
                break;
            case "Victory":
                {
                    if((int)Time.deltaTime % 2 == 0)
                    {
                        tEffect = Managers.Resource.Instantiate("Effect_Victory1").GetComponent<ParticleSystem>();
                    }
                    else
                    {
                        tEffect = Managers.Resource.Instantiate("Effect_Victory2").GetComponent<ParticleSystem>();
                    }
                    tEffect.transform.position = position;
                }
                break;
            case "StatueExplosion":
                {
                    tEffect = Managers.Resource.Instantiate("Effect_StatueExplosion").GetComponent<ParticleSystem>();
                    tEffect.transform.position = position;
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! PlayEffect(): Wrong Name!!!");
                }
                break;
        }
    }
}