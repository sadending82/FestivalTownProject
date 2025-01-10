using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Customize_ItemGrid : UI_Base
{
    int currentWidth = 1160;
    int itemWidth = 225;
    List<UI_Customize_Item> _items = new();

    public override void Init()
    {
        
    }

    public void AddItem(ref UI_Customize_Item item)
    {
        _items.Add(item);
        item.transform.SetParent(transform);

        if (_items.Count <= 0) GetComponent<RectTransform>().sizeDelta = new Vector2(currentWidth, 0);
        else
        {
            // 아이템 갯수가 충분하면 확장해 주어야 함.
            // int / int 는 버림으로 나옴.
            GetComponent<RectTransform>().sizeDelta = new Vector2(currentWidth, ((((_items.Count) * itemWidth) / currentWidth) + 1) * itemWidth);
        }
    }

    public int GetHeight()
    {
        return ((((_items.Count) * itemWidth) / currentWidth) + 1) * itemWidth;
    }

    
}
