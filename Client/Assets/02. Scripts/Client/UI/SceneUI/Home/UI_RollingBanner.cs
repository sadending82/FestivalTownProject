using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_RollingBanner : UI_Base
{
    enum GameObjects
    {
        Banners,
        BannerTabs,
    }

    bool isInitialized = false;
    int currentSelected = 0;
    float Timer = 0.0f;
    float RollTime = 8.0f;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        GameObject banners = Get<GameObject>((int)GameObjects.Banners);

        banners.transform.GetChild(0).gameObject.BindEvent((PointerEventData) =>
        {
            Managers.UI.ShowPopUpUI<UI_DailySignInCheck>();
        });

        banners.transform.GetChild(1).gameObject.BindEvent((PointerEventData) =>
        {
            // 개발자의 편지가 없어...
        });

        var ui = Get<GameObject>((int)GameObjects.BannerTabs).GetComponent<UI_BannerTabs>();
        ui.Init();
        ui.SetParentUI(this);

        SetBannerOn(0);

        isInitialized = true;
    }

    public void SetBannerOn(int idx)
    {
        for (int i = 0; i < 2; ++i)
        {
            Get<GameObject>((int)GameObjects.Banners).transform.GetChild(i).gameObject.SetActive(false);
        }

        Get<GameObject>((int)GameObjects.Banners).transform.GetChild(idx).gameObject.SetActive(true);
        Get<GameObject>((int)GameObjects.BannerTabs).GetComponent<UI_BannerTabs>().SetSelected(idx);

        currentSelected = idx;
        Timer = 0.0f;
    }

    void Update()
    {
        Timer += Time.deltaTime;
        if(Timer > RollTime)
        {
            Timer -= RollTime;
            switch(currentSelected)
            {
                case 0:
                    SetBannerOn(1);
                    break;
                case 1:
                    SetBannerOn(0);
                    break;
            }
        }
    }
}
