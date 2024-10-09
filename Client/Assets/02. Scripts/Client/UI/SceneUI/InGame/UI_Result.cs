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

    bool isResultChecked = false;
    void Start()
    {
        Init();
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
        });
    }

    public void SetPlayerResult(int playerId, int kill, int death, int bombInsert, int gold, bool isMvp)
    {
        Get<GameObject>((int)GameObjects.ResultPanel).GetComponent<UI_ResultPanel>().SetPlayerResult(playerId, kill, death, bombInsert, gold, isMvp);
    }

    private void Update()
    {
        if (Managers.Game.PlayerResultData.Count > 0 && !isResultChecked)
        {
            foreach (KeyValuePair<int, Define.PlayerResult> data in Managers.Game.PlayerResultData)
            {
                SetPlayerResult(data.Key, data.Value.kill, data.Value.death,
                    data.Value.bombInsert, data.Value.gold, data.Value.isMvp);
            }

            isResultChecked = true;
        }
    }

}
