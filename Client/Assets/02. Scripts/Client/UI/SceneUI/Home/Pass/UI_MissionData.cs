using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using Unity.VisualScripting;

public class UI_MissionData : UI_Base
{
    enum GameObjects
    {
        RecvButton,
        ExpData,
        MissionTitle,
        MissionDescription,
        RewardPanel,
    }

    bool isInitialized = false;
    string baseTitle = "";
    int missionIdx = -1;
    int countedTimes = 0;
    int required = 0;
    Define.MissionType _type;
    bool isRewarded = false;

    void Start()
    {
        transform.localScale = Vector3.one;

        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.RecvButton).BindEvent((PointerEventData) =>
        {
            if(isRewarded == false && missionIdx != -1)
            {
                Managers.Network.GetPacketManager().SendMissionCompleteResponsePacket(missionIdx);
                Debug.Log($"{missionIdx} Mission Complete Response Packet Send.");
            }
        });

        isInitialized = true;
    }

    public void SetMissionIndex(int idx)
    {
        missionIdx = idx;
    }

    public int GetMissionIndex()
    {
        return missionIdx;
    }

    public void SetExp(int exp)
    {
        Get<GameObject>((int)GameObjects.ExpData).transform.GetChild(1).GetComponent<TMP_Text>().text = $"+{exp}p";
    }

    public void SetTitle(string title)
    {
        baseTitle = title;
        Get<GameObject>((int)GameObjects.MissionTitle).GetComponent<TMP_Text>().text = title + $" ({countedTimes} / {required})";
    }

    public void SetDescription(string descr)
    {
        Get<GameObject>((int)GameObjects.MissionDescription).GetComponent<TMP_Text>().text = descr;
    }

    public void SetReward(int rewardIdx, int amount)
    {
        var ImageTr = Get<GameObject>((int)GameObjects.RewardPanel).transform.GetChild(0);
        var TextTr = Get<GameObject>((int)GameObjects.RewardPanel).transform.GetChild(1);

        Managers.Data.ItemDict.TryGetValue(rewardIdx, out var itemData);

        ImageTr.GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
        TextTr.GetComponent<TMP_Text>().text = $"+{amount}";
    }

    public void SetType(Define.MissionType type)
    {
        _type = type;

        if(_type == Define.MissionType.PassMission)
        {
            Get<GameObject>((int)GameObjects.RewardPanel).SetActive(false);
        }
        else
        {
            Get<GameObject>((int)GameObjects.RewardPanel).SetActive(true);
        }
    }

    public void SetRequired(int required)
    {
        this.required = required;
    }

    public void SetMissionCounted(int counted)
    {
        countedTimes = counted;

        SetTitle(baseTitle);
    }

    public void SetRewarded(bool isRewarded)
    {
        this.isRewarded = isRewarded;

        if(this.isRewarded)
        {
            Get<GameObject>((int)GameObjects.RecvButton).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("Completed");
            gameObject.GetComponent<Image>().color = Color.gray;
        }
        else
        {
            Get<GameObject>((int)GameObjects.RecvButton).GetComponent<Image>().sprite = Managers.Resource.LoadSprite("DoComplete");
            gameObject.GetComponent<Image>().color = Color.white;
        }
    }

    public bool IsRewarded()
    {
        return isRewarded;
    }
}
