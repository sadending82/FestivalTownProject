using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Customize_ScrollView : UI_Base
{
    enum GameObjects
    {
        Viewport,
        ScrollbarVertical,
    }

    bool isInitialized = false;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetItem(ref UI_Customize_Item item, Define.ItemType type)
    {
        if(!isInitialized) Init();

        Transform Content = Get<GameObject>((int)GameObjects.Viewport).transform.GetChild(0); // �̰� Content
        Transform itemGrid = Content.GetComponent<UI_Customize_ScrViewContent>().GetGridTransformByItemType(type); //Content ������ ItemGrid�� �޾ƿ��� ���� �༮
        itemGrid.GetComponent<UI_Customize_ItemGrid>().AddItem(ref item); // �ش� ItemGrid�� ������ �߰�
        item.transform.SetParent(itemGrid); // �ش� itemGrid�� item�г��� �θ�� �ξ� ���� ���� ���� �����ϰ� ����.
    }

    public GameObject GetContent()
    {
        return Get<GameObject>((int)GameObjects.Viewport).transform.GetChild(0).gameObject;
    }
}
