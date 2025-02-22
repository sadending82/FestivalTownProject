using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class UI_Shop : UI_Scene
{

    enum GameObjects
    {
        BG,
        CategorySelector,
        ScrollView
    }

    Dictionary<int, UI_ShopItem> ShopItemDataDict = new();
    bool isInitialzied = false;

    void Start()
    {
        if(!isInitialzied) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();

        Transform contentTf = GetContentOfScrollView();
        
        for (int i = (int)Define.StartIdx.ShopListMileageStartIdx; Managers.Data.ShopListDataDict.ContainsKey(i); ++i)
        {
            var shopListData = Managers.Data.ShopListDataDict[i];

            var ui = Managers.UI.MakeSubItem<UI_ShopItem>(contentTf.GetChild(1));
            ui.Init();
            bool isAlreadOwned = Managers.Data.InventoryDataList.ContainsKey(shopListData.Item);
            ui.SetItemData(shopListData.Item, i, shopListData.Currency1_Id, shopListData.Currency1_Price, isAlreadOwned, false);
            ShopItemDataDict.Add(i, ui);
        }

        isInitialzied = true;
    }

    Transform GetContentOfScrollView()
    {
        // ScrollView πÿ¿« Viewport πÿ¿« Content∏¶ return
        return Get<GameObject>((int)GameObjects.ScrollView).transform.GetChild(0).GetChild(0);
    }

    public UI_ShopItem GetShopItem(int idx)
    {
        ShopItemDataDict.TryGetValue(idx, out var ui);
        return ui;
    }

    private void Update()
    {
        GetContentOfScrollView().GetChild(0).GetComponent<UI_ShopClassifier>().SetCurrentCurrency(Managers.Data.GetMileage());
    }
}
