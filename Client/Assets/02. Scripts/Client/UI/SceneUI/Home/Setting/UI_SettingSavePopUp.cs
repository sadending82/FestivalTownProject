using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using static UnityEngine.Rendering.DebugUI;

public class UI_SettingSavePopUp : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        Panel,
        NoticeText,
        OkButton,
        NoButton,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            PlayerPrefs.SetFloat("Sound_Master_Volume", Managers.Sound._masterVolume);
            PlayerPrefs.SetFloat("Sound_Eff_Volume", Managers.Sound._effVolume);
            PlayerPrefs.SetFloat("Sound_Bgm_Volume", Managers.Sound._bgmVolume);
            PlayerPrefs.SetFloat("Sound_Cat_Volume", Managers.Sound._catVolume);

            PlayerPrefs.Save();
            Managers.UI.ClosePopUpUI(this);
        });

        Get<GameObject>((int)GameObjects.NoButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

        isInitialized = true;
    }
}
