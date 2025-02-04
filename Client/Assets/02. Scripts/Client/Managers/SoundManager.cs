using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;
using static Unity.VisualScripting.Member;

/// <summary>
/// ���� ���� �Ŵ���
/// 3D ����� ���� ���ϰ�, 2D ���常 �����ϵ��� �մϴ�./
/// 3D ����� �Ÿ��� ���⿡ ���� �Ҹ��� �޶����� �ϱ� ������ �ٸ� ������� �ϴ°� �½��ϴ�.
/// </summary>
public class SoundManager
{
    AudioMixer mAudioMixer;

    // 2d ���� ������ ���� ������
    AudioSource[] _audioSources = new AudioSource[3];
    Dictionary<string, AudioClip> _audioClips = new Dictionary<string, AudioClip>();

    // 3d ���� ������ ���� ������
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

            string[] sounds = System.Enum.GetNames(typeof(Define.Sound)); // ���� ���÷����ΰ� �ƽð���?
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
    // ���� ������ �� �Ű��� �� �κ�����
    //
    // �⺻������ �ִ� ���� 0���� 1������ ������ �ְ� ����.
    // PlayerPrefs�� �� => 0~1
    // �ͼ� ������ �� = > m~M
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
                    // Bgm�� ��� ��ø�Ǿ� ����ϴ� ���� ����� �ϹǷ�
                    // �̹� ������̸� ���߰� ���ο�� ����
                    AudioSource source = _audioSources[(int)Define.Sound.Bgm];
                    if (source.isPlaying) source.Stop();

                    source.clip = audioClip;
                    source.volume = 1;
                    source.Play();
                }
                break;
            case Define.Sound.Effect:
                {
                    // ȿ������ ��� ��ø�Ǿ ���� �����Ƿ�
                    // �׳� ����ϱ�
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
                    // Bgm�� ��� ��ø�Ǿ� ����ϴ� ���� ����� �ϹǷ�
                    // �̹� ������̸� ���߰� ���ο�� ����
                    AudioSource source = _audioSources[(int)Define.Sound.Bgm];
                    source.Stop();
                }
                break;
            case Define.Sound.Effect:
                {
                    // ȿ������ ��� ��ø�Ǿ ���� �����Ƿ�
                    // �׳� ����ϱ�
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
        // �׻� Sounds �����ȿ� �ֱ�!
        if (path.Contains("Sounds/") == false)
            path = $"Sounds/{path}";

        AudioClip clip = null;

        switch(type)
        {
            case Define.Sound.Bgm:
                // bgm�� �ѹ� �ε��ϰ� ��� �����Ŵϱ� �׳� �̷��� �ϸ� ��.
                clip = Managers.Resource.Load<AudioClip>(path);
                break;
            case Define.Sound.Effect:
                // ȿ������ ���� ���Ǳ� ������ �ε��� ����ϸ� ������尡 �Ǵϱ�, 
                // �̷� ������ �־� ���� ����ϱ�
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

        // ���� �ܰ踦 ��� ���ƴµ� �ƹ��͵� �� �������� �׳� ���尡 ���°���

        if (clip == null)
            Debug.Log($"{path}�� AudioClip�� ������...");

        return clip;
    }
}
