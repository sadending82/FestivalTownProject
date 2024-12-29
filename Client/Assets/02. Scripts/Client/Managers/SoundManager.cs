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
    AudioSource[] _audioSources = new AudioSource[2];
    Dictionary<string, AudioClip> _audioClips = new Dictionary<string, AudioClip>();

    // 3d ���� ������ ���� ������
    Dictionary<GameObject, AudioSource> _3dAudioSource = new();



    public float _masterVolume = 1.0f;
    public float _bgmVolume = 1.0f;
    public float _effVolume = 1.0f;
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
        }
    }

    public void SetMasterVolume(float value)
    {
        mAudioMixer.SetFloat("Master", value);
    }

    public void SetEffectVolume(float value)
    {
        mAudioMixer.SetFloat("Effect", value);
    }

    public void SetBgmVolume(float value)
    {
        mAudioMixer.SetFloat("BGM", value);
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
            _3dAudioSource.Add(sourceObj, source);
        }

        AudioClip clip = GetOrAddAudioClip(path, type);

        if (clip == null) return;

        switch(type)
        {
            case Define.Sound.Bgm:
                if (source.isPlaying) source.Stop();

                source.clip = clip;
                source.volume = _bgmVolume;
                source.Play();
                break;
            case Define.Sound.Effect:
                source.volume = _effVolume;
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
            default:
                break;
        }

        // ���� �ܰ踦 ��� ���ƴµ� �ƹ��͵� �� �������� �׳� ���尡 ���°���

        if (clip == null)
            Debug.Log($"{path}�� AudioClip�� ������...");

        return clip;
    }
}
