using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_Login : UI_Scene
{
    enum GameObjects
    {
        BackGroundImage,
        TitleLogo,
        LoginPanel,
        QuitButton,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.QuitButton).BindEvent((PointerEventData) =>
        {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
        });

        GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
    }

    public void Login()
    {
        Get<GameObject>((int)GameObjects.LoginPanel).GetComponent<UI_LoginPanel>().Login();
    }


}
