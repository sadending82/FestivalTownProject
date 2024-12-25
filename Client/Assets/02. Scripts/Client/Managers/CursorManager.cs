using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class CursorManager : MonoBehaviour
{
    Texture2D cursorTexture;
    Texture2D cursorClickedTexture;
    Vector3 cursorHotspot;
    

    void Start()
    {
        cursorTexture = Managers.Resource.LoadTexture("Cursor");
        cursorClickedTexture = Managers.Resource.LoadTexture("CursorClicked");
        cursorHotspot = new Vector2(cursorTexture.width / 2, cursorTexture.height / 2);
        Cursor.SetCursor(cursorTexture, cursorHotspot, CursorMode.Auto);
    }

    private void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            Cursor.SetCursor(cursorClickedTexture, cursorHotspot, CursorMode.Auto);
        }

        if (Input.GetMouseButtonUp(0))
        {
            Cursor.SetCursor(cursorTexture, cursorHotspot, CursorMode.Auto);
        }
    }
}
