using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_DailyProgress : UI_Base
{
    enum GameObjects
    {
        LabelText,
        ProgressBar,
    }

    bool isInitialized = false;
    void Start()
    {
        transform.localScale = Vector3.one;

        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetProgress(int progress)
    {
        var pbObj = Get<GameObject>((int)GameObjects.ProgressBar);
        pbObj.transform.GetChild(1).GetComponent<TMP_Text>().text = $"{progress} / 100";
        pbObj.GetComponent<Slider>().value = progress / (float)100.0f;
    }
}
