using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 사운드 제어 매니저
/// 3D 사운드는 관리 안하고, 2D 사운드만 관리하도록 합니다./
/// 3D 사운드는 거리와 방향에 따라 소리가 달라져야 하기 때문에 다른 방식으로 하는게 맞습니다.
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

            string[] sounds = System.Enum.GetNames(typeof(Define.Sound)); // 이젠 리플렉션인거 아시겠죠?
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
                    // Bgm의 경우 중첩되어 재생하는 경우는 없어야 하므로
                    // 이미 재생중이면 멈추고 새로운거 실행
                    AudioSource source = _audioSources[(int)Define.Sound.Bgm];
                    if (source.isPlaying) source.Stop();

                    source.clip = audioClip;
                    source.Play();
                }
                break;
            case Define.Sound.Effect:
                {
                    // 효과음의 경우 중첩되어도 문제 없으므로
                    // 그냥 재생하기
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
            default:
                break;
        }

        // 위의 단계를 모두 거쳤는데 아무것도 안 나왔으면 그냥 사운드가 없는거임

        if (clip == null)
            Debug.Log($"{path}에 AudioClip이 업서용...");

        return clip;
    }
}
