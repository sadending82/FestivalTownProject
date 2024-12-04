using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_DailyCheckItemVertLayout : UI_Base
{
    enum GameObjects
    {
        Day,
        ItemImage,
        Name,
    }

    bool isInitialized = false;

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
        Get<GameObject>((int)GameObjects.Day).GetComponent<TMP_Text>().text = $"Day {day}";
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

    public void BindEvent(int dataIndex)
    {
        Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(0).gameObject.BindEvent((PointerEventData) =>
        {
            Debug.Log($"selecte {dataIndex}");
            // 여기서 선택한 아이템에 대한 데이터를 보내주어야 할 듯.
            // 데이터 보내주고 나서 패킷 처리 단계에서 받아와야 한다.
            // Managers.Network.GetPacketManager().SendAttendanceEventRequestPacket();
        });
    }
}
