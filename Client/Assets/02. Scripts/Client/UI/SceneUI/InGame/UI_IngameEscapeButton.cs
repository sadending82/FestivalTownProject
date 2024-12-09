using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_IngameEscapeButton : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        NoticePanel,
        OkButton,
        NoticeText,
        ExitButton,
    }

    bool isInitialized = false;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            if(!Managers.Game.isGameEnd)
            {
                Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;
            }
            ClosePopUpUI();
        });

        isInitialized = true;
    }

    public void NoticeTextChange(string str)
    {
        GameObject go = Get<GameObject>((int)GameObjects.NoticeText);
        if (go == null)
        {
            StartCoroutine(TextChangeAfterStart(str));
        }
        else { go.GetComponent<TMP_Text>().text = str; }
    }

    IEnumerator TextChangeAfterStart(string str)
    {
        yield return null;
        Get<GameObject>((int)GameObjects.NoticeText).GetComponent<TMP_Text>().text = str;
    }

    public void BindPopupCloseEvent()
    {
        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            ClosePopUpUI();
        });

    }

    public void BindGameEndEvent()
    {
        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) => {

            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;

#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
        });
    }
}
