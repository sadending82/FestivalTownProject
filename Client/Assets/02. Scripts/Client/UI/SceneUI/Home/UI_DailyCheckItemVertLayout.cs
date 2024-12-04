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
            // 0�� => ������ �̹���
            // 1�� => �޾Ҵ� �� üũ�� �̹���
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(true);
        }
        else
        {
            // 0�� => ������ �̹���
            // 1�� => �޾Ҵ� �� üũ�� �̹���
            Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(1).gameObject.SetActive(false);
        }
    }

    public void BindEvent(int dataIndex)
    {
        Get<GameObject>((int)GameObjects.ItemImage).transform.GetChild(0).gameObject.BindEvent((PointerEventData) =>
        {
            Debug.Log($"selecte {dataIndex}");
            // ���⼭ ������ �����ۿ� ���� �����͸� �����־�� �� ��.
            // ������ �����ְ� ���� ��Ŷ ó�� �ܰ迡�� �޾ƿ;� �Ѵ�.
            // Managers.Network.GetPacketManager().SendAttendanceEventRequestPacket();
        });
    }
}
