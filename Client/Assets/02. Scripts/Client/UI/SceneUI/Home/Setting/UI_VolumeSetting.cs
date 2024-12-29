using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_VolumeSetting : UI_Base
{
    enum GameObjects
    {
        VolumeText,
        VolumeSlider,
        VolumeVisual,
    }

    bool isInitialized = false;
    Define.Sound soundType;

    public void Start()
    {
        if (!isInitialized) Init();
    }

    public void SetSoundType(Define.Sound value)
    {
        soundType = value;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void Update()
    {
        float sliderValue = Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value;

        switch (soundType)
        {
            case Define.Sound.Bgm:

                Managers.Sound._bgmVolume = sliderValue;
                Managers.Sound.SetBgmVolume(sliderValue);
                Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                    ((int)((1.0f + (1.0f / 80.0f * sliderValue)) * 100.0f)).ToString();
                break;
            case Define.Sound.Effect:
                Managers.Sound._effVolume = Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value;
                Managers.Sound.SetEffectVolume(Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value);
                Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                    ((int)((1.0f + (1.0f / 80.0f * sliderValue)) * 100.0f)).ToString();
                break;
            default:
                break;
        }
    }

}
