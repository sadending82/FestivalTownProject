using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// ���� ���� �Ŵ���
/// 3D ����� ���� ���ϰ�, 2D ���常 �����ϵ��� �մϴ�./
/// 3D ����� �Ÿ��� ���⿡ ���� �Ҹ��� �޶����� �ϱ� ������ �ٸ� ������� �ϴ°� �½��ϴ�.
/// </summary>
public class SoundManager
{
    AudioSource[] _audioSources = new AudioSource[2];
    Dictionary<string, AudioClip> _audioClips = new Dictionary<string, AudioClip>();

    public void Init()
    {
        GameObject root = GameObject.Find("@Sound");
        if (root == null)
        {
            root = new GameObject { name = "@Sound" };
            Object.DontDestroyOnLoad(root);

            string[] sounds = System.Enum.GetNames(typeof(Define.Sound)); // ���� ���÷����ΰ� �ƽð���?
            for (int i = 0; i < sounds.Length - 1; ++i)
            {
                GameObject go = new GameObject { name = sounds[i] };
                _audioSources[i] = go.AddComponent<AudioSource>();
                go.transform.SetParent(root.transform);
            }

            _audioSources[(int)Define.Sound.Bgm].loop = true;
        }
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
                    source.Play();
                }
                break;
            case Define.Sound.Effect:
                {
                    // ȿ������ ��� ��ø�Ǿ ���� �����Ƿ�
                    // �׳� ����ϱ�
                    AudioSource source = _audioSources[(int)Define.Sound.Effect];
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
