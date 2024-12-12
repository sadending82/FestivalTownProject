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

        Transform Content = Get<GameObject>((int)GameObjects.Viewport).transform.GetChild(0); // 이게 Content
        Transform itemGrid = Content.GetComponent<UI_Customize_ScrViewContent>().GetGridTransformByItemType(type); //Content 내부의 ItemGrid를 받아오기 위한 녀석
        itemGrid.GetComponent<UI_Customize_ItemGrid>().AddItem(ref item); // 해당 ItemGrid에 아이템 추가
        item.transform.SetParent(itemGrid); // 해당 itemGrid를 item패널의 부모로 두어 둘이 서로 소통 가능하게 만듬.
    }

    public GameObject GetContent()
    {
        return Get<GameObject>((int)GameObjects.Viewport).transform.GetChild(0).gameObject;
    }
}
