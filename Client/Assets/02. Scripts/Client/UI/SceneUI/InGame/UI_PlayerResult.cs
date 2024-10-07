using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_PlayerResult : UI_Base
{
    enum GameObjects
    {
        MVPEmphasis,
        NickNameText,
        KillText,
        DeathText,
        BombInsertText,
        GoldText,
        MVPImage,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
    }

    public void SetPlayerResult(int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        if (Get<GameObject>((int)GameObjects.KillText) == null) Debug.Log("text null");
        if (Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>() == null) Debug.Log("tmp null");
        Debug.Log(Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>().text);
        Debug.Log($"kill = {kill}");
        Get<GameObject>((int)GameObjects.KillText).GetComponent<TMP_Text>().text = kill.ToString();

        Get<GameObject>((int)GameObjects.DeathText).GetComponent<TMP_Text>().text = death.ToString();
        Get<GameObject>((int)GameObjects.BombInsertText).GetComponent<TMP_Text>().text = bombInsert.ToString();
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = gold.ToString();

        if (!isMvp)
        {
            Get<GameObject>((int)GameObjects.MVPEmphasis).SetActive(false);
            Get<GameObject>((int)GameObjects.MVPImage).SetActive(false);
        }
    }
}
