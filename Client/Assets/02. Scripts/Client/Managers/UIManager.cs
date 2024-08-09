using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager
{
    int _order = 10; // 마지막 놈이 몇번째 순서인지 기억하기 위한 거. 점점 늘어남

    Stack<UI_PopUp> _popupStack = new Stack<UI_PopUp>(); // UI를 스택에 담아두면 뺄때 편하겠지?
    UI_Scene _sceneUI = null; // 팝업으로 여는거 말고 고정되어 있는 놈이 누구임

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
        canvas.overrideSorting = true; // 캔버스 두 개면 위에 겹쳐버리라구~

        if (sort)
        {
            canvas.sortingOrder = _order;
            _order++;
        }
        else // 소팅 안하면 그냥 고정되는 놈이라 맨 앞에 놓으면 됨
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
            Debug.LogWarning("젤 위에 있는 놈이 님이 지우려고 했던거 하고 다른데요;; 나 안해 때려쳐");
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
