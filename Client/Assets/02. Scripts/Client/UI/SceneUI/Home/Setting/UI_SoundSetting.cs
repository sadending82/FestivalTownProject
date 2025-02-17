using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_SoundSetting : UI_Base
{
    enum GameObjects
    {
        MasterVolumeSetting,
        CatVoiceVolumeSetting,
        EffectVolumeSetting,
        BGMVolumeSetting,
    }

    bool isInitialized = false;

    public void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
        var bgm = Get<GameObject>((int)GameObjects.BGMVolumeSetting).GetComponent<UI_VolumeSetting>();
        bgm.Init();
        bgm.SetSoundType(Define.Sound.Bgm);

        var eff = Get<GameObject>((int)GameObjects.EffectVolumeSetting).GetComponent<UI_VolumeSetting>();
        eff.Init();
        eff.SetSoundType(Define.Sound.Effect);

        var cat = Get<GameObject>((int)GameObjects.CatVoiceVolumeSetting).GetComponent<UI_VolumeSetting>();
        cat.Init();
        cat.SetSoundType(Define.Sound.Cat);

        var master = Get<GameObject>((int)GameObjects.MasterVolumeSetting).GetComponent<UI_VolumeSetting>();
        master.Init();
        master.SetMasterSound();

        isInitialized = true;
    }

    public void SetDefaults()
    {
        foreach (GameObjects gobj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)gobj).GetComponent<UI_VolumeSetting>().SetDefault();
        }

    }

    public void OnDestroy()
    {
        Managers.Sound.SoundSettingFromPrefs();
    }
}
