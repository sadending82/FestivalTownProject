using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager
{
    int _order = 10; // ������ ���� ���° �������� ����ϱ� ���� ��. ���� �þ

    Stack<UI_PopUp> _popupStack = new Stack<UI_PopUp>(); // UI�� ���ÿ� ��Ƶθ� ���� ���ϰ���?
    UI_Scene _sceneUI = null; // �˾����� ���°� ���� �����Ǿ� �ִ� ���� ������

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

    public void SetCanvas(GameObject go, bool sort = true)
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

    public void CloseAllPopUpUI()
    {
        while (_popupStack.Count > 0)
        {
            ClosePopUpUI();
        }
    }
}
