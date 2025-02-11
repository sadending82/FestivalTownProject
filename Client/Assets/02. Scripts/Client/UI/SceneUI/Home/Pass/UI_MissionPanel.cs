using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UIElements;

public class UI_MissionPanel : UI_Base
{
    enum GameObjects
    {
        Title,
        MissionExplainText,
        MissionCategorySelector,
        MissionCategory,
    }

    bool isInitialized = false;
    private List<string> explainTexts = new() { "���� �̼��� ���� 00�ÿ� �ʱ�ȭ�˴ϴ�.", "�н� �̼��� �н� �Ⱓ���� 1ȸ Ŭ���� �����մϴ�." };

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        var mcUI = Get<GameObject>((int)GameObjects.MissionCategory).GetComponent<UI_MissionCategory>();
        mcUI.Init();

        var mcsUI = Get<GameObject>((int)GameObjects.MissionCategorySelector).GetComponent<UI_MissionCategorySelector>();
        mcsUI.Init();

        mcsUI.SetParentUI(this);

        SelectCategory(mcsUI.GetSelected());

        isInitialized = true;
    }

    public void SelectCategory(int idx)
    {
        var mcsUI = Get<GameObject>((int)GameObjects.MissionCategorySelector).GetComponent<UI_MissionCategorySelector>();

        mcsUI.SetSelected(idx);

        var mcUI = Get<GameObject>((int)GameObjects.MissionCategory).GetComponent<UI_MissionCategory>();

        mcUI.SetCategoryOn(idx);

        ChangeMissionExplainText(explainTexts[idx]);
    }

    public void ChangeMissionExplainText(string str)
    {
        Get<GameObject>((int)GameObjects.MissionExplainText).GetComponent<TMP_Text>().text = str;
    }

    public Transform GetCategoryContent(int idx)
    {
        var mcUI = Get<GameObject>((int)GameObjects.MissionCategory).GetComponent<UI_MissionCategory>();
        return mcUI.GetCategoryContent(idx);
    }
}
