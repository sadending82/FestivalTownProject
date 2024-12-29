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

        var Eff = Get<GameObject>((int)GameObjects.EffectVolumeSetting).GetComponent<UI_VolumeSetting>();
        Eff.Init();
        Eff.SetSoundType(Define.Sound.Effect);

        isInitialized = true;
    }
}
