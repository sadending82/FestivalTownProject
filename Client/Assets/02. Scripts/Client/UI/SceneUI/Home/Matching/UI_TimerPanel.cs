using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_TimerPanel : UI_Base
{

    float matchingTime = 0.0f;

    enum GameObjects
    {
        LabelText,
        TimerText,
        QuitButton,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetTimer()
    {
        int min = (int)(matchingTime / 60.0f);
        int sec = (int)(matchingTime % 60.0f);

        Get<GameObject>((int)GameObjects.TimerText).GetComponent<TMP_Text>().text = string.Format("{0:D}:{1:D2}", min, sec);
    }

    private void Update()
    {
        matchingTime += Time.deltaTime;
        SetTimer();
    }
}
