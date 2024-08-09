using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager
{
    int _order = 10;

    Stack<UI_PopUp> _popupStack = new Stack<UI_PopUp>();
    UI_Scene _sceneUI = null;

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
        else
        {
            canvas.sortingOrder = 0;
        }
    }
}
