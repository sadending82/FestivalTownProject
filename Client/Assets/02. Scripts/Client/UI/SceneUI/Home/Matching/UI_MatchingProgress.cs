using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_MatchingProgress : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        WhiteBoard,
        QuitButton,
        TimerPanel,
    }

    bool isInitialized = false;

    // Start is called before the first frame update
    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.QuitButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendGameMatchingCancel();
        });

        isInitialized = true;
    }

    private void Update()
    {
        if(Input.GetKeyUp(KeyCode.Escape))
        {
            Managers.Network.GetPacketManager().SendGameMatchingCancel();
        }
    }
}
