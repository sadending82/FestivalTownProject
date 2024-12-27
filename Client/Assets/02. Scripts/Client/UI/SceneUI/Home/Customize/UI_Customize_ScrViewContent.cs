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
    /// ������ Ÿ�Կ� ���� ��� �־�� ���� �з��ϱ� ���� �Լ�.
    /// ������ ���� ��ġ�� ������.
    /// </summary>
    /// <param name="type">������ Ÿ��</param>
    /// <returns>�ش� �������� �� Transform</returns>
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
