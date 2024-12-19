using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_GachaResult : UI_PopUp
{
    enum GameObjects
    {
        Blocker,
        NoticePanel,
        OkButton,
        NoticeText,
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

        Managers.Sound.Play("Sfx_Popup_In");

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
            Managers.UI.CloseGachaCutSceneUI(GetComponent<UI_GachaResult>());
        });
    }
}
