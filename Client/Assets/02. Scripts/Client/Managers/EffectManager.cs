using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EffectManager : MonoBehaviour
{
    [Header("--- Object ---")]
    public ParticleSystem BombExplosionEffect;
    public ParticleSystem CubeFalldownEffect;

    [Header("--- Character ---")]
    public ParticleSystem CharacterGroggyEffect;
    public ParticleSystem CharacterRespawnEffect;

    [Header("--- Other ---")]
    public ParticleSystem VictorySceneEffect;

    public void LoadPrefabs()
    {
        //Objects
        BombExplosionEffect = Managers.Resource.Instantiate("Effect_BombExplosion").GetComponent<ParticleSystem>();
        CubeFalldownEffect = Managers.Resource.Instantiate("Effect_CubeFalldown").GetComponent<ParticleSystem>();

        //Character
        CharacterGroggyEffect = Managers.Resource.Instantiate("Effect_Ch_Groggy").GetComponent<ParticleSystem>();
        CharacterRespawnEffect = Managers.Resource.Instantiate("Effect_Ch_Respawn").GetComponent<ParticleSystem>();
        
        //Other
        VictorySceneEffect = Managers.Resource.Instantiate("Effect_Victory1").GetComponent<ParticleSystem>();
        VictorySceneEffect = Managers.Resource.Instantiate("Effect_Victory2").GetComponent<ParticleSystem>();
    }
}