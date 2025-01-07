using NetworkProtocol;
using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_Loading : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        TipDescriptionText,
        ModeTitleText,
        ModeDescriptionText,
        ProgressBar,
        IndivOrTeamImage,
        PlayerCntImage,
    }


    void Start()
    {
        Init(); 
    }

    public void SetProgressBarPersentage(float progress)
    {
        Get<GameObject>((int)GameObjects.ProgressBar).GetComponent<Slider>().value = progress;
    }

    public void SetTipDescription(string text)
    {
        Get<GameObject>((int)GameObjects.TipDescriptionText).GetComponent<TMP_Text>().text = text;
    }

    public void SetNewTipDescription()
    {
        int randomNum = UnityEngine.Random.Range(0, Managers.Game.TipIndices.Count);

        bool result = Managers.Data.ModeTipDataDict.TryGetValue(Managers.Game.TipIndices[randomNum], out var tipData);
        Debug.Log($"newTipResult {result}");
        if (result)
        {
            Managers.Game.TipDescription = tipData.Tip_Description;
        }

        SetTipDescription(Managers.Game.TipDescription);
    }

    public void SetModeTitle(string text)
    {
        Get<GameObject>((int)GameObjects.ModeTitleText).GetComponent<TMP_Text>().text = text;
    }

    public void SetModeDescriptionText(string text)
    {
        Get<GameObject>((int)GameObjects.ModeDescriptionText).GetComponent<TMP_Text>().text = text;
    }

    public void SetThemeImage(int mapTheme)
    {
        switch (mapTheme)
        {
            case 1:
                Get<GameObject>((int)GameObjects.Panel).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("CatTemple_Day");
                break;
            case 2:
                Get<GameObject>((int)GameObjects.Panel).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("CatTemple_Night");
                break;
        }
    }

    private void SetGameMode(eGameMode gameMode)
    {
        switch(gameMode)
        {
            case eGameMode.FITH_Indiv_Battle_2:
                Get<GameObject>((int)GameObjects.IndivOrTeamImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("IndivGame");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("1vs1");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().SetNativeSize();
                break;
            case eGameMode.FITH_Indiv_Battle_3:
                Get<GameObject>((int)GameObjects.IndivOrTeamImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("IndivGame");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("1vs1vs1");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().SetNativeSize();
                break;
            case eGameMode.FITH_Team_Battle_4:
                Get<GameObject>((int)GameObjects.IndivOrTeamImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("TeamGame");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("2vs2");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().SetNativeSize();
                break;
            case eGameMode.FITH_Team_Battle_6:
                Get<GameObject>((int)GameObjects.IndivOrTeamImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("TeamGame");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("3vs3");
                Get<GameObject>((int)GameObjects.PlayerCntImage).GetComponent<Image>().SetNativeSize();
                break;
        }
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        SetModeTitle(Managers.Game.ModeTitle);
        SetModeDescriptionText(Managers.Game.ModeDescription);
        SetThemeImage(Managers.Game.mapTheme);
        SetGameMode(Managers.Game.gameMode);

        int randomNum = UnityEngine.Random.Range(0, Managers.Game.TipIndices.Count);

        bool result = Managers.Data.ModeTipDataDict.TryGetValue(Managers.Game.TipIndices[randomNum], out var tipData);
        Debug.Log($"newTipResult {result}");
        if (result)
        {
            Managers.Game.TipDescription = tipData.Tip_Description;
        }

        SetTipDescription(Managers.Game.TipDescription);

        foreach (var objs in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)objs).BindEvent((PointerEventData) =>
            {
                SetNewTipDescription();
            });
        }

        
    }
}
