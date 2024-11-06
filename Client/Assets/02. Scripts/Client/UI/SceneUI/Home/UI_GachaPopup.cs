using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_GachaPopup : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        Image,
        Text,
        OkButton,
        NoButton,
    }

    int GachaType;

    void Start()
    {

        base.Init();
        Init();

    }

    public void SetGachaType(int value)
    {
        GachaType = value;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.OkButton).BindEvent((PointerEventData) =>
        {
            Debug.Log($"{GachaType} ±×·ì");
            Managers.Network.GetPacketManager().SendGachaRequestPacket(GachaType);
            Managers.UI.ClosePopUpUI(this);
        });

        Get<GameObject>((int)GameObjects.NoButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI(this);
        });

    }
}
