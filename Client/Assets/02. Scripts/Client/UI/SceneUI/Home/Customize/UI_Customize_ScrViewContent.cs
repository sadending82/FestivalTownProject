using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Customize_ScrViewContent : UI_Base
{
    enum GameObjects
    {
        SkinClassifier,
        SkinItems,
        HeadClassifier,
        HeadItems,
        FaceClassifier,
        FaceItems,
        BackClassifier,
        BackItems,
        EmoClassifier,
        EmoItems,
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
    /// 아이템 타입에 따라 어디에 넣어야 할지 분류하기 위한 함수.
    /// 아이템 넣을 위치를 돌려줌.
    /// </summary>
    /// <param name="type">아이템 타입</param>
    /// <returns>해당 아이템이 들어갈 Transform</returns>
    public Transform GetGridTransformByItemType(Define.ItemType type)
    {
        if(!isInitialized) Init();

        switch(type)
        {
            case Define.ItemType.Skin:
                return Get<GameObject>((int)GameObjects.SkinItems).transform;
            case Define.ItemType.Accessory_Head:
                return Get<GameObject>((int)GameObjects.HeadItems).transform;
            case Define.ItemType.Accessory_Face:
                return Get<GameObject>((int)GameObjects.FaceItems).transform;
            case Define.ItemType.Accessory_Back:
                return Get<GameObject>((int)GameObjects.BackItems).transform;
            case Define.ItemType.Emo:
                return Get<GameObject>((int)GameObjects.EmoItems).transform;
            default:
                break;
        }

        return null;
    }

    public void SetHeightByContents()
    {
        if (!isInitialized) Init();

        int myHeight = 600; 

        myHeight += Get<GameObject>((int)GameObjects.SkinItems).GetComponent<UI_Customize_ItemGrid>().GetHeight();
        myHeight += Get<GameObject>((int)GameObjects.HeadItems).GetComponent<UI_Customize_ItemGrid>().GetHeight();
        myHeight += Get<GameObject>((int)GameObjects.FaceItems).GetComponent<UI_Customize_ItemGrid>().GetHeight();
        myHeight += Get<GameObject>((int)GameObjects.BackItems).GetComponent<UI_Customize_ItemGrid>().GetHeight();
        myHeight += Get<GameObject>((int)GameObjects.EmoItems).GetComponent<UI_Customize_ItemGrid>().GetHeight();

        GetComponent<RectTransform>().sizeDelta = new Vector2(GetComponent<RectTransform>().sizeDelta.x, myHeight);
    }
}
