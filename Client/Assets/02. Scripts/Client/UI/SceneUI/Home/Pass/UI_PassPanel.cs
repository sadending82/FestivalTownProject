using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI_PassPanel : UI_Base
{
    enum GameObjects
    {
        PassLevelPanel,
        PassItemPanel,
        GetAllItemButton,
    }

    bool isInitialized = false;
    int passIndex = -1;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.PassLevelPanel).GetComponent<UI_PassLevelPanel>().Init();
        Get<GameObject>((int)GameObjects.PassItemPanel).GetComponent<UI_PassItemPanel>().Init();
        Get<GameObject>((int)GameObjects.GetAllItemButton).BindEvent((PointerEventData) =>
        {
            Managers.Network.GetPacketManager().SendBatchReceivePassRewardsRequestPacket(passIndex);
        });

        isInitialized = true;
    }

    public void SetPassIdx(int passIdx)
    {
        passIndex = passIdx;
    }

    public void SetLevel(int level)
    {
        Get<GameObject>((int)GameObjects.PassLevelPanel).GetComponent<UI_PassLevelPanel>().SetLevel(level);
    }

    public void SetProgress(int exp, int maxExp)
    {
        Get<GameObject>((int)GameObjects.PassLevelPanel).GetComponent<UI_PassLevelPanel>().SetProgress(exp, maxExp);
    }

    public UI_PassLevelItem AddItem(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        return Get<GameObject>((int)GameObjects.PassItemPanel).GetComponent<UI_PassItemPanel>().AddItem(basicItemIdx, plusItemIdx, basicPassListIdx, plusPassListIdx, level);
    }


}
