using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_DailyCheckItemVertLayout : UI_Base
{
    enum GameObjects
    {
        Image,
        Day,
        ItemImage,
        Name,
    }

    bool isInitialized = false;
    UI_DailyCheckItem parentUI = null;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetDay(int day)
    {
        Get<GameObject>((int)GameObjects.Day).GetComponent<TMP_Text>().text = $"{day} 일차";
    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.Name).GetComponent<TMP_Text>().text = $"{name}";
    }

    public void SetItemRewarded(bool rewarded)
    {
        if(rewarded)
        {
            // 0번 => 아이템 이미지
            // 1번 => 받았는 지 체크용 이미지
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(true);
        }
        else
        {
            // 0번 => 아이템 이미지
            // 1번 => 받았는 지 체크용 이미지
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(false);
        }
    }

    public void SetItemImage(Sprite sp)
    {
        Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(0).GetComponent<Image>().sprite = sp;
    }

    public void SetParentUI(UI_DailyCheckItem ui)
    {
        parentUI = ui;
    }

    public void BindEvent(int dataIndex)
    {
        Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(0).gameObject.BindEvent((PointerEventData) =>
        {
            if(parentUI.IsAquireable())
            {
                Managers.Network.GetPacketManager().SendAttendanceRewardRequestPacket(parentUI.GetEventCode(), parentUI.GetDay());
                Debug.Log("Send");
            }
        });
    }
}
