using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_ItemInfoPanel : UI_Base
{
    enum GameObjects
    {
        ItemInfoList
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

    /// <summary>
    /// 인덱스 넣으면 해당 아이템의 데이터 집어 넣는 함수
    /// </summary>
    /// <param name="index">GachaGroup_Item의 인덱스</param>
    public void AddItemInfoList(int index)
    {
        GameObject infoList = Get<GameObject>((int)GameObjects.ItemInfoList);
        var ui = Managers.UI.MakeSubItem<UI_ItemInfo>(infoList.transform);
        ui.Init();
        if(Managers.Data.ItemDict.TryGetValue(index, out var itemEntity))
        {
            ui.SetItem(itemEntity.Index);
            ui.SetItemTexture((Define.ItemGrade)itemEntity.Item_Grade);
        }

    }
}
