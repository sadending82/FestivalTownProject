using System.Collections;
using System.Collections.Generic;
using UnityEditor.Rendering;
using UnityEngine;

public class UI_GachaType : UI_Base
{
    enum GameObjects
    {
        Name,
        Image,
        LeftButton,
        RightButton,
        GachaButton,
    }

    void Start()
    {
        Init();

    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.RightButton).BindEvent((PointerEventData)=>
        {
            Debug.Log("이전 버튼");
            transform.parent.GetComponent<UI_GachaSelectPanel>().SelectNextGacha();
        });

        Get<GameObject>((int)GameObjects.LeftButton).BindEvent((PointerEventData) =>
        {
            Debug.Log("이후 버튼");
            transform.parent.GetComponent<UI_GachaSelectPanel>().SelectPrevGacha();
        });

        Get<GameObject>((int)GameObjects.GachaButton).BindEvent((PointerEventData) =>
        {
            // 가챠 실행
        });
    }
}
