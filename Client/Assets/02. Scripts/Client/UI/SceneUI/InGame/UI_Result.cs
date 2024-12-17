using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_Result : UI_Scene
{
    enum GameObjects
    {
        PanelBG,
        Logo,
        ResultPanel,
        RestartButton,
        BackToLobbyButton,

    }
    bool isInitialized = false;
    bool isResultChecked = false;
    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.RestartButton).BindEvent((PointerEventData) => {
            Debug.Log($"다시하기 버튼을 누르셨군요!");
        });

        Get<GameObject>((int)GameObjects.BackToLobbyButton).BindEvent((PointerEventData) =>
        {
            Managers.Scene.LoadScene(Define.Scene.Home);
            Managers.ObjectPool.Clear();
            Managers.Game.Clear();
            Managers.CubeObject.Clear();
            Managers.Player.Clear();
            Managers.SpectatorCamera.Clear();
            Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
        });

        UI_ResultPanel[] panels = Get<GameObject>((int)GameObjects.ResultPanel).transform.GetComponentsInChildren<UI_ResultPanel>();

        foreach(UI_ResultPanel pan in panels)
        {
            pan.Init();
        }

        isInitialized = true;
    }

    public void SetPlayerResult(string nickname, int playerId, int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        Get<GameObject>((int)GameObjects.ResultPanel).GetComponent<UI_ResultPanel>().SetPlayerResult(nickname, playerId, kill, death, bombInsert, gold, isMvp);
    }

    private void Update()
    {
        if (Managers.Game.PlayerResultData.Count > 0 && !isResultChecked)
        {
            foreach (var data in Managers.Game.PlayerResultData)
            {
                SetPlayerResult(data.Name, data.Id, data.kill, data.death,
                    data.bombInsert, data.gold, data.isMvp);
            }

            isResultChecked = true;
        }
    }

}
