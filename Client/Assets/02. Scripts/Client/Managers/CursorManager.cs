using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CursorManager : MonoBehaviour
{
    Texture2D cursorTexture;
    Texture2D cursorClickedTexture;
    Vector3 cursorHotspot;
    

    void Start()
    {
        cursorTexture = Managers.Resource.LoadTexture("CursorTemp");
        cursorHotspot = new Vector2(cursorTexture.width / 2, cursorTexture.height / 2);
        Cursor.SetCursor(cursorTexture, cursorHotspot, CursorMode.Auto);
    }
}
