using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public abstract class UI_Base : MonoBehaviour
{
    //--------------------------------------------------------------------------
    //
    // UI_Base는 모든 UI들의 기본이 되는 클래스 입니다
    // 기본적으로 모든 UI가 이 클래스를 상속받아 진행할 것입니다.
    // Unity에서 abstract는 추상화를 하겠다는 이야기인거 아시죠?
    //
    //--------------------------------------------------------------------------

    protected Dictionary<Type, UnityEngine.Object[]> _objects = new Dictionary<Type, UnityEngine.Object[]>();
    public abstract void Init();


    /// <summary>
    /// 기본적으로 유니티 씬 상의 오브젝트들을 로드해서 바인딩하여 보관하기 위한 함수
    /// 바로바로 접근하려고 만든거라구~
    /// </summary>
    /// <typeparam name="T">제네릭 프로그래밍 OK?</typeparam>
    /// <param name="type">
    /// 오브젝트의 이름을 바탕으로 종류별로 구분한 Enum의 Reflection이 들어갑니다
    /// Reflection이 무엇인지는 검색해보시지요 히히
    /// </param>
    protected void Bind<T>(Type type) where T : UnityEngine.Object // T 라는 타입은, UnityEngine의 Object의 타입 중 하나입니다~ 라는 뜻
    {
        string[] names = Enum.GetNames(type);

        UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
        _objects.Add(typeof(T), objects);

        for (int i = 0; i < names.Length; i++)
        {
            if (typeof(T) == typeof(GameObject))
            {
                objects[i] = Util.FindChild(gameObject, names[i], true);           
            }
            else
            {
                objects[i] = Util.FindChild<T>(gameObject, names[i], true);
            }

            if (objects[i] == null)
                Debug.LogWarning($"{names[i]} 바인드 실패");
        }
    }


    /// <summary>
    /// T에 해당하는 타입을 바탕으로 해당 T 타입의 index 번째 오브젝트를 반환
    /// </summary>
    /// <typeparam name="T">뭔 타입이십니까?</typeparam>
    /// <param name="index">몇번째 이십니까?</param>
    /// <returns></returns>
    protected T Get<T>(int index) where T : UnityEngine.Object
    {
        UnityEngine.Object[] objects = null;
        if (_objects.TryGetValue(typeof(T), out objects) == false)
            return null;

        return objects[index] as T;
    }

    protected GameObject GetObject(int idx) { return Get<GameObject>(idx); }
    protected Text GetText(int idx) { return Get<Text>(idx); }
    protected TMP_Text GetTextTMP(int idx) { return Get<TMP_Text>(idx); }
    protected Button GetButton(int idx) { return Get<Button>(idx); }
    protected Image GetImage(int idx) { return Get<Image>(idx); }



    public static void BindEvent(GameObject go, Action<PointerEventData> action, Define.UIEvent type = Define.UIEvent.Click)
    {
        UI_EventHandler eventHandler = Util.GetOrAddComponent<UI_EventHandler>(go);

        switch(type)
        {
            case Define.UIEvent.Click:
                eventHandler.OnClickHandler -= action;
                eventHandler.OnClickHandler += action;
                break;
            case Define.UIEvent.Drag:
                eventHandler.OnDragHandler -= action;
                eventHandler.OnDragHandler += action;
                break;
        }
    }

}
