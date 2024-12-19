using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class UIManager
{
    int _order = 10; // ������ ���� ���° �������� ����ϱ� ���� ��. ���� �þ
    int _dictKeyOrder = 0; // WorldSpace �� Dictionary Key �ο��� ���� ����. �̰ŵ� ���� �þ

    Stack<UI_PopUp> _popupStack = new Stack<UI_PopUp>(); // UI�� ���ÿ� ��Ƶθ� ���� ���ϰ���?
    Dictionary<int, UI_WorldSpace> _worldList = new ();
    UI_Scene _sceneUI = null; // �˾����� ���°� ���� �����Ǿ� �ִ� ���� ������
    UI_AlwaysOnTop _AlwaysOnTopUI = null; // �׻� ���� ���� �־�� �ϴ� ���� ������

    public GameObject Root
    {
        get
        {
            GameObject root = GameObject.Find("@UI_Root");
            if (root == null)
            {
                root = new GameObject { name = "@UI_Root" };
            }

            return root;
        }
    }

    public void SetCanvas(GameObject go, bool sort = true, bool alwaysOnTop = false)
    {
        Canvas canvas = Util.GetOrAddComponent<Canvas>(go);
        canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.overrideSorting = true; // ĵ���� �� ���� ���� ���Ĺ�����~

        if (sort)
        {
            canvas.sortingOrder = _order;
            _order++;
        }
        else // ���� ���ϸ� �׳� �����Ǵ� ���̶� �� �տ� ������ ��
        {
            canvas.sortingOrder = 0;
        }

        if (alwaysOnTop)
        {
            canvas.sortingOrder = 99;
        }
    }

    public T ShowSceneUI<T>(string name = null) where T : UI_Scene
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Resource.Instantiate($"UI/Scene/{name}");

        T sceneUI = Util.GetOrAddComponent<T>(go);

        _sceneUI = sceneUI;

        go.transform.SetParent(Root.transform);

        return sceneUI;
    }

    public void CloseSceneUI()
    {
        if (_sceneUI == null) return;

        Managers.Resource.Destroy(_sceneUI.gameObject);
        _sceneUI = null;
    }

    public T ShowPopUpUI<T>(string name = null) where T : UI_PopUp
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Resource.Instantiate($"UI/PopUp/{name}");

        T popup = Util.GetOrAddComponent<T>(go);
        _popupStack.Push(popup);

        go.transform.SetParent(Root.transform);

        return popup;
    }

    public UI_PopUp GetTopOfPopUPUI()
    {
        if (_popupStack.Count == 0) return null;

        UI_PopUp pop = _popupStack.Peek();

        return pop;
    }

    public UI_Scene GetCurrentSceneUI()
    {
        return _sceneUI;
    }

    public void ClosePopUpUI(UI_PopUp popUp)
    {
        if (_popupStack.Count == 0) return;

        if (_popupStack.Peek() != popUp)
        {
            Debug.LogWarning("�� ���� �ִ� ���� ���� ������� �ߴ��� �ϰ� �ٸ�����;; �� ���� ������");
            return;
        }

        ClosePopUpUI();
    }

    public void ClosePopUpUI()
    {
        if (_popupStack.Count == 0) return;

        UI_PopUp popUp = _popupStack.Pop();
        Managers.Resource.Destroy(popUp.gameObject);
        popUp = null;
        _order--;
    }

    public void CloseGachaCutSceneUI(UI_PopUp popUp)
    {
        if (_popupStack.Count == 0) return;

        if (_popupStack.Peek() != popUp)
        {
            Debug.LogWarning("�Ծ� ��í �ƾ� ���°� ����?");
            return;
        }
        // ���߿� �����ؾ� ���� ������
        while (_popupStack.Count > 2)
        {
            ClosePopUpUI();
        }
    }

    public void CloseAllPopUpUI()
    {
        while (_popupStack.Count > 0)
        {
            ClosePopUpUI();
        }
    }

    /// <summary>
    /// �׻� ���� ���� �ִ� (Always On Top) UI�� �����
    /// </summary>
    /// <typeparam name="T">UI Ÿ��</typeparam>
    /// <param name="name">UI �̸�</param>
    /// <returns></returns>
    public T ShowAOTUI<T>(string name = null) where T : UI_AlwaysOnTop
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Resource.Instantiate($"UI/AlwaysOnTop/{name}");

        T aotUI = Util.GetOrAddComponent<T>(go);

        _AlwaysOnTopUI = aotUI;

        go.transform.SetParent(Root.transform);

        return aotUI;
    }

    /// <summary>
    /// AlwaysOnTopUI ����
    /// </summary>
    public void CloseAOTUI()
    {
        if (_AlwaysOnTopUI == null) return;
        Managers.Resource.Destroy(_AlwaysOnTopUI.gameObject);
        _AlwaysOnTopUI = null;
    }

    public T MakeWorldSpace<T>(Transform parent = null, string name = null) where T : UI_WorldSpace
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Resource.Instantiate($"UI/WorldSpace/{name}");
        if (parent != null)
            go.transform.SetParent(parent);

        Canvas canvas = go.GetComponent<Canvas>();
        canvas.renderMode = RenderMode.WorldSpace;
        canvas.worldCamera = Camera.main;

        T worldSpace = Util.GetOrAddComponent<T>(go);
        worldSpace.SetKey(_dictKeyOrder++);

        return worldSpace;
    }

    public T MakeSubItem<T>(Transform parent = null, string name = null) where T : UI_Base
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Resource.Instantiate($"UI/SubItem/{name}");

        if (parent != null)
            go.transform.SetParent(parent);

        return Util.GetOrAddComponent<T>(go);
    }

    public void Clear()
    {
        CloseAllPopUpUI();
        CloseAOTUI();
        CloseSceneUI();
        
    }
}
