using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_HomeScene : UI_PopUp
{
    enum GameObjects
    {
        GoldPanel,
        GoldText,
        DiamondPanel,
        DiamondText,
        TopPanel,
        PlayerDataPanel,
        SubMenuButton,
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

        SetNickName(Managers.Data.GetNickName());
        SetGold(Managers.Data.GetGold());
        SetDiamond(Managers.Data.GetDiamond());

        Get<GameObject>((int)GameObjects.SubMenuButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_SubMenu>();
        });

        isInitialized = true;
    }

    public void SetDiamond(int value)
    {
        Get<GameObject>((int)GameObjects.DiamondText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public void SetGold(int value)
    {
        Get<GameObject>((int)GameObjects.GoldText).GetComponent<TMP_Text>().text = value.ToString();
    }

    public void SetNickName(string value)
    {
        Get<GameObject>((int)GameObjects.PlayerDataPanel).transform.GetChild(0).GetComponent<TMP_Text>().text = value;
    }

    private void Update()
    {
        SetGold(Managers.Data.GetGold());
        SetDiamond(Managers.Data.GetDiamond());
    }
}
