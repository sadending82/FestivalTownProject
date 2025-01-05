using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HomeScene : BaseScene
{
    UI_HomeScene homeSceneUI;
    UI_HomeStart homeStartUI;
    public bool isMatching = false;
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Home;

        homeStartUI = Managers.UI.ShowSceneUI<UI_HomeStart>();
        homeSceneUI = Managers.UI.ShowPopUpUI<UI_HomeScene>();

        homeStartUI.Init();
        homeSceneUI.Init();

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    public void Start()
    {
        if (PlayerPrefs.HasKey("Sound_Master_Volume"))
        {
            Managers.Sound.SetMasterVolume(PlayerPrefs.GetFloat("Sound_Master_Volume"));
        }

        if (PlayerPrefs.HasKey("Sound_Bgm_Volume"))
        {
            Managers.Sound.SetBgmVolume(PlayerPrefs.GetFloat("Sound_Bgm_Volume"));
        }

        if (PlayerPrefs.HasKey("Sound_Eff_Volume"))
        {
            Managers.Sound.SetEffectVolume(PlayerPrefs.GetFloat("Sound_Eff_Volume"));
        }

        if (PlayerPrefs.HasKey("Sound_Cat_Volume"))
        {
            Managers.Sound.SetCatVolume(PlayerPrefs.GetFloat("Sound_Cat_Volume"));
        }

        // home���� �������� �� 1������ ���� �� ũ�� �鸮�� ��찡 �־� �ڷ�ƾ���� �о�α�
        StartCoroutine(StartBGM());
    }

    IEnumerator StartBGM()
    {
        yield return null;
        Managers.Sound.Play("Sfx_BGM_Lobby", Define.Sound.Bgm);
        yield break;
    }

    public override void Clear()
    {
        
    }
}
