using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResourceManager
{
    public T Load<T>(string path) where T : Object
    {
        return Resources.Load<T>(path);
    }

    public GameObject Instantiate(string path, Transform parent = null)
    {
        
        GameObject prefab = Load<GameObject>($"03. Prefabs/{path}");

        if(prefab == null)
        {
            Debug.LogWarning($"¾Æ´Ï {path}¿¡ ÇÁ¸®ÆÕ ¾øÀÝ¾Æ¿ä;;");
            return null;
        }

        return Object.Instantiate(prefab, parent);
    }

    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        Object.Destroy(go);
    }
}
