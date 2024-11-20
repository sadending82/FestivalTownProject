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
    // UI_Base�� ��� UI���� �⺻�� �Ǵ� Ŭ���� �Դϴ�
    // �⺻������ ��� UI�� �� Ŭ������ ��ӹ޾� ������ ���Դϴ�.
    // Unity���� abstract�� �߻�ȭ�� �ϰڴٴ� �̾߱��ΰ� �ƽ���?
    //
    //--------------------------------------------------------------------------

    protected Dictionary<Type, UnityEngine.Object[]> _objects = new Dictionary<Type, UnityEngine.Object[]>();
    public abstract void Init();


    /// <summary>
    /// �⺻������ ����Ƽ �� ���� ������Ʈ���� �ε��ؼ� ���ε��Ͽ� �����ϱ� ���� �Լ�
    /// �ٷιٷ� �����Ϸ��� ����Ŷ�~
    /// </summary>
    /// <typeparam name="T">���׸� ���α׷��� OK?</typeparam>
    /// <param name="type">
    /// ������Ʈ�� �̸��� �������� �������� ������ Enum�� Reflection�� ���ϴ�
    /// Reflection�� ���������� �˻��غ������� ����
    /// </param>
    protected void Bind<T>(Type type) where T : UnityEngine.Object // T ��� Ÿ����, UnityEngine�� Object�� Ÿ�� �� �ϳ��Դϴ�~ ��� ��
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
                Debug.LogWarning($"{names[i]} ���ε� ����");
        }
    }


    /// <summary>
    /// T�� �ش��ϴ� Ÿ���� �������� �ش� T Ÿ���� index ��° ������Ʈ�� ��ȯ
    /// </summary>
    /// <typeparam name="T">�� Ÿ���̽ʴϱ�?</typeparam>
    /// <param name="index">���° �̽ʴϱ�?</param>
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
