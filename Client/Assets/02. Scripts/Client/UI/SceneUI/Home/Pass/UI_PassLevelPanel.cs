using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_PassLevelPanel : UI_Base
{
    enum GameObjects
    {
        PassLevelFrame,
        Level,
        ProgressBar,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetLevel(int level)
    {
        Get<GameObject>((int)GameObjects.Level).GetComponent<TMP_Text>().text = $"패스 레벨 {level}";
    }

    public void SetProgress(int exp, int maxExp)
    {
        float value = exp / (float)maxExp;

        GameObject pb = Get<GameObject>((int)GameObjects.ProgressBar);

        pb.GetComponent<Slider>().value = value;
        pb.transform.GetChild(1).GetComponent<TMP_Text>().text = $"{exp} / {maxExp}";
    }

}
