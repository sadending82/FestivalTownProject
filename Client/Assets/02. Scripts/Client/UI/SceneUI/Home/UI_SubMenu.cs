using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_SubMenu : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        ExitButton,
        DiscordButton,
        GameEndButton,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

        Get<GameObject>((int)GameObjects.DiscordButton).BindEvent((PointerEventData) =>
        {
            //디스코드 관련
        });

        Get<GameObject>((int)GameObjects.GameEndButton).BindEvent((PointerEventData) =>
        {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
            Application.Quit();
#endif
        });

        isInitialized = true;
    }
}
