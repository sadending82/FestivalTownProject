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
    bool isMasterVolume = false;

    public void Start()
    {
        if (!isInitialized) Init();
    }

    public void SetSoundType(Define.Sound value)
    {
        soundType = value;

        switch (value)
        {
            case Define.Sound.Bgm:
                Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._bgmVolume;
                break;
            case Define.Sound.Effect:
                Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._effVolume;
                break;
            case Define.Sound.Cat:
                Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._catVolume;
                break;
        }
    }

    public void SetMasterSound()
    {
        Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._masterVolume;
        isMasterVolume = true;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        

        isInitialized = true;
    }

    public void SetDefault()
    {
        if(isMasterVolume)
        {
            Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._masterVolume;
        }
        else
        {
            switch (soundType)
            {
                case Define.Sound.Bgm:
                    Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._bgmVolume;
                    break;
                case Define.Sound.Effect:
                    Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._effVolume;
                    break;
                case Define.Sound.Cat:
                    Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value = Managers.Sound._catVolume;
                    break;
            }
        }
    }

    public void Update()
    {
        float sliderValue = Get<GameObject>((int)GameObjects.VolumeSlider).GetComponent<Slider>().value;

        if (isMasterVolume)
        {
            Managers.Sound._masterVolume = sliderValue;
            Managers.Sound.SetMasterVolume(sliderValue);
            Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                ((int)(sliderValue * 100.0f)).ToString();
        }
        else
        {
            switch (soundType)
            {
                case Define.Sound.Bgm:

                    Managers.Sound._bgmVolume = sliderValue;
                    Managers.Sound.SetBgmVolume(sliderValue);
                    Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                        ((int)(sliderValue * 100.0f)).ToString();
                    break;
                case Define.Sound.Effect:
                    Managers.Sound._effVolume = sliderValue;
                    Managers.Sound.SetEffectVolume(sliderValue);
                    Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                        ((int)(sliderValue * 100.0f)).ToString();
                    break;
                case Define.Sound.Cat:
                    Managers.Sound._catVolume = sliderValue;
                    Managers.Sound.SetCatVolume(sliderValue);
                    Get<GameObject>((int)GameObjects.VolumeVisual).GetComponent<TMP_InputField>().text =
                        ((int)(sliderValue * 100.0f)).ToString();
                    break;
                default:
                    break;
            }
        }
    }

}
