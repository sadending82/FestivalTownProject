using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;
using static Unity.VisualScripting.Member;

/// <summary>
/// 사운드 제어 매니저
/// 3D 사운드는 관리 안하고, 2D 사운드만 관리하도록 합니다./
/// 3D 사운드는 거리와 방향에 따라 소리가 달라져야 하기 때문에 다른 방식으로 하는게 맞습니다.
/// </summary>
public class SoundManager
{
    AudioMixer mAudioMixer;

    // 2d 사운드 관리를 위한 데이터
    AudioSource[] _audioSources = new AudioSource[3];
    Dictionary<string, AudioClip> _audioClips = new Dictionary<string, AudioClip>();

    // 3d 사운드 관리를 위한 데이터
    Dictionary<GameObject, AudioSource> _3dAudioSource = new();

    LogarithmicRangeConverter converter = new LogarithmicRangeConverter(-80.0f, -15.0f, 0.0f);

    public float _masterVolume = 1.0f;
    public float _bgmVolume = 1.0f;
    public float _effVolume = 1.0f;
    public float _catVolume = 1.0f;
    public void Init()
    {
        GameObject root = GameObject.Find("@Sound");
        if (root == null)
        {
            root = new GameObject { name = "@Sound" };
            UnityEngine.Object.DontDestroyOnLoad(root);

            string[] sounds = System.Enum.GetNames(typeof(Define.Sound)); // 이젠 리플렉션인거 아시겠죠?
            for (int i = 0; i < sounds.Length; ++i)
            {
                GameObject go = new GameObject { name = sounds[i] };
                _audioSources[i] = go.AddComponent<AudioSource>();
                go.transform.SetParent(root.transform);
            }

            _audioSources[(int)Define.Sound.Bgm].loop = true;

            mAudioMixer = Managers.Resource.Load<AudioMixer>("AudioMixer/SoundMixer");
            AudioMixerGroup[] mixerGroups = mAudioMixer.FindMatchingGroups("Master");
            _audioSources[(int)Define.Sound.Bgm].outputAudioMixerGroup = mixerGroups[3];
            _audioSources[(int)Define.Sound.Effect].outputAudioMixerGroup = mixerGroups[2];
            _audioSources[(int)Define.Sound.Cat].outputAudioMixerGroup = mixerGroups[1];
        }

        InitSoundSetting();

    }

    //--------------------------------------------------------------------------
    //
    // 볼륨 조절할 때 신경써야 할 부분으로
    //
    // 기본적으로 주는 값은 0에서 1사이의 값으로 주고 싶음.
    // PlayerPrefs의 값 => 0~1
    // 믹서 내부의 값 = > m~M
    // 
    //--------------------------------------------------------------------------

    public LogarithmicRangeConverter GetConverter()
    {
        return converter;
    }

    public void InitSoundSetting()
    {
        SetPlayerPrefsVolumeByKey("Sound_Master_Volume");
        SetPlayerPrefsVolumeByKey("Sound_Bgm_Volume");
        SetPlayerPrefsVolumeByKey("Sound_Eff_Volume");
        SetPlayerPrefsVolumeByKey("Sound_Cat_Volume");
    }

    public void SetPlayerPrefsVolumeByKey(string key)
    {
        if (PlayerPrefs.HasKey(key))
        {
            if (PlayerPrefs.GetFloat(key) > 1.0f || PlayerPrefs.GetFloat(key) < 0.0f)
            {
                PlayerPrefs.DeleteKey(key);
            }
            else
            {
                switch(key)
                {
                    case "Sound_Master_Volume":
                        SetMasterVolume(PlayerPrefs.GetFloat(key));
                        break;
                    case "Sound_Bgm_Volume":
                        SetEffectVolume(PlayerPrefs.GetFloat(key));
                        break;
                    case "Sound_Eff_Volume":
                        SetBgmVolume(PlayerPrefs.GetFloat(key));
                        break;
                    case "Sound_Cat_Volume":
                        SetCatVolume(PlayerPrefs.GetFloat(key));
                        break;
                    default:
                        break;
                }
            }
        }
    }

    public void SetMasterVolume(float value)
    {
        float actualValue = converter.ToRealRange(value);
        mAudioMixer.SetFloat("Master", actualValue);
        _masterVolume = value;
    }

    public void SetEffectVolume(float value)
    {
        float actualValue = converter.ToRealRange(value);
        mAudioMixer.SetFloat("Effect", actualValue);
        _effVolume = value;
    }

    public void SetBgmVolume(float value)
    {
        float actualValue = converter.ToRealRange(value);
        mAudioMixer.SetFloat("BGM", actualValue);
        _bgmVolume = value;
    }

    public void SetCatVolume(float value)
    {
        float actualValue = converter.ToRealRange(value);
        mAudioMixer.SetFloat("CatVoice", actualValue);
        _catVolume = value;
    }

    public void SetDefaults()
    {
        _masterVolume = Defaults.MASTER_VOLUME_DEFAULT;
        _effVolume = Defaults.EFFECT_VOLUME_DEFAULT;
        _catVolume = Defaults.CAT_VOLUME_DEFAULT;
        _bgmVolume = Defaults.BGM_VOLUME_DEFAULT;

        SetMasterVolume(_masterVolume);
        SetEffectVolume(_effVolume);
        SetBgmVolume(_bgmVolume);
        SetCatVolume(_catVolume);

        PlayerPrefs.SetFloat("Sound_Master_Volume", Managers.Sound._masterVolume);
        PlayerPrefs.SetFloat("Sound_Eff_Volume", Managers.Sound._effVolume);
        PlayerPrefs.SetFloat("Sound_Bgm_Volume", Managers.Sound._bgmVolume);
        PlayerPrefs.SetFloat("Sound_Cat_Volume", Managers.Sound._catVolume);
    }

    public void Clear()
    {
        
        foreach (AudioSource audioSource in _audioSources)
        {
            if (audioSource == null) continue;
            audioSource.clip = null;
            audioSource.Stop();
        }

        _audioClips.Clear();
    }


    public void Play(AudioClip audioClip, Define.Sound type = Define.Sound.Effect)
    {
        if (audioClip == null) return;

        

        switch(type)
        {
            case Define.Sound.Bgm:
                {
                    // Bgm의 경우 중첩되어 재생하는 경우는 없어야 하므로
                    // 이미 재생중이면 멈추고 새로운거 실행
                    AudioSource source = _audioSources[(int)Define.Sound.Bgm];
                    if (source.isPlaying) source.Stop();

                    source.clip = audioClip;
                    source.volume = 1;
                    source.Play();
                }
                break;
            case Define.Sound.Effect:
                {
                    // 효과음의 경우 중첩되어도 문제 없으므로
                    // 그냥 재생하기
                    AudioSource source = _audioSources[(int)Define.Sound.Effect];
                    source.volume = 1;
                    source.PlayOneShot(audioClip);
                }
                break;
            case Define.Sound.Cat:
                {
                    AudioSource source = _audioSources[(int)Define.Sound.Cat];
                    source.volume = 1;
                    source.PlayOneShot(audioClip);
                }
                break;
            default:
                break;
        }
    }

    public void Play(string path, Define.Sound type = Define.Sound.Effect)
    {
        AudioClip clip = GetOrAddAudioClip(path, type);
        Play(clip, type);
    }

    public void Play3D(string path, GameObject sourceObj, Define.Sound type = Define.Sound.Effect)
    {
        AudioSource source = null;

        if (_3dAudioSource.TryGetValue(sourceObj, out source) == false)
        {
            source = Util.GetOrAddComponent<AudioSource>(sourceObj);
            switch (type)
            {
                case Define.Sound.Effect:
                    {
                        source.outputAudioMixerGroup = mAudioMixer.FindMatchingGroups("Master")[2];
                    }
                    break;
                case Define.Sound.Cat:
                    {
                        source.outputAudioMixerGroup = mAudioMixer.FindMatchingGroups("Master")[1];
                    }
                    break;
                case Define.Sound.Bgm:
                    {
                        source.outputAudioMixerGroup = mAudioMixer.FindMatchingGroups("Master")[3];
                    }
                    break;
            }
            _3dAudioSource.Add(sourceObj, source);
            
        }

        AudioClip clip = GetOrAddAudioClip(path, type);

        if (clip == null) return;

        switch(type)
        {
            case Define.Sound.Bgm:
                if (source.isPlaying) source.Stop();

                source.clip = clip;
                source.volume = 1.0f;
                source.Play();
                break;
            case Define.Sound.Effect:
                source.volume = 1.0f;
                source.PlayOneShot(clip);
                break;
            case Define.Sound.Cat:
                source.volume = 1.0f;
                source.PlayOneShot(clip);
                break;
            default:
                break;
        }
    }

    public void Stop(Define.Sound type = Define.Sound.Effect)
    {
        switch (type)
        {
            case Define.Sound.Bgm:
                {
                    // Bgm의 경우 중첩되어 재생하는 경우는 없어야 하므로
                    // 이미 재생중이면 멈추고 새로운거 실행
                    AudioSource source = _audioSources[(int)Define.Sound.Bgm];
                    source.Stop();
                }
                break;
            case Define.Sound.Effect:
                {
                    // 효과음의 경우 중첩되어도 문제 없으므로
                    // 그냥 재생하기
                    AudioSource source = _audioSources[(int)Define.Sound.Effect];
                    source.Stop();
                }
                break;
            case Define.Sound.Cat:
                {
                    AudioSource source = _audioSources[(int)Define.Sound.Cat];
                    source.Stop();
                }
                break;
            default:
                break;
        }
    }

    public void Stop3D(GameObject sourceObj)
    {
        AudioSource source;

        if (_3dAudioSource.TryGetValue(sourceObj, out source) == false) return;

        source.Stop();
    }

    AudioClip GetOrAddAudioClip(string path, Define.Sound type = Define.Sound.Effect)
    {
        // 항상 Sounds 폴더안에 넣기!
        if (path.Contains("Sounds/") == false)
            path = $"Sounds/{path}";

        AudioClip clip = null;

        switch(type)
        {
            case Define.Sound.Bgm:
                // bgm은 한번 로딩하고 계속 돌릴거니까 그냥 이렇게 하면 됨.
                clip = Managers.Resource.Load<AudioClip>(path);
                break;
            case Define.Sound.Effect:
                // 효과음은 자주 사용되기 때문에 로딩을 계속하면 오버헤드가 되니까, 
                // 이런 식으로 넣어 놓고 사용하기
                if (_audioClips.TryGetValue(path, out clip) == false)
                {
                    clip = Managers.Resource.Load<AudioClip>(path);
                    _audioClips.Add(path, clip);
                }
                break;
            case Define.Sound.Cat:
                if (_audioClips.TryGetValue(path, out clip) == false)
                {
                    clip = Managers.Resource.Load<AudioClip>(path);
                    _audioClips.Add(path, clip);
                }
                break;
            default:
                break;
        }

        // 위의 단계를 모두 거쳤는데 아무것도 안 나왔으면 그냥 사운드가 없는거임

        if (clip == null)
            Debug.Log($"{path}에 AudioClip이 업서용...");

        return clip;
    }
}
