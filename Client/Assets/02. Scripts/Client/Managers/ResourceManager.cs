using UnityEngine;

public class ResourceManager
{
    
    public T Load<T>(string path) where T : Object
    {
        // 로드할 때 오브젝트 풀링이 되는 지 안 되는지를 파악해서 없애야 하니까 이거도 고쳐야 함.

        // 오브젝트 풀에 있으면 그냥 가져오기
        if (typeof(T) == typeof(GameObject))
        {
            string name = path;
            int index = name.LastIndexOf('/'); // "/"의 뒤에서 이름 가져오기
            if (index >= 0)
                name = name.Substring(index + 1);

            GameObject go = Managers.ObjectPool.GetOrigin(name);
            if (go != null)
            {
                return go as T;
            }
        }

        // 오브젝트 풀에 없으면 로딩하기
        return Resources.Load<T>(path);
    }

    public Texture2D LoadTexture(string path)
    {
        Texture2D texture = Load<Texture2D>($"Textures/{path}");

        if(texture == null)
        {
            Debug.LogWarning($"{path}에 텍스쳐가 없잖아!");
            return null;
        }

        return texture;
    }

    public Sprite LoadSprite(string path)
    {
        Sprite sprite = Load<Sprite>($"Sprite/{path}");

        if (sprite == null)
        {
            Debug.LogWarning($"{path}에 스프라이트가 없어요...");
            return null;
        }

        return sprite;
    }

    public GameObject Instantiate(string path, Transform parent = null)
    {
        GameObject prefab = Load<GameObject>($"Prefabs/{path}");

        if(prefab == null)
        {
            Debug.LogWarning($"아니 {path}에 프리팹 없잖아요;;");
            return null;
        }

        // 오브젝트 풀에 있으면 굳이 만들 필요 없음
        if (prefab.GetComponent<Poolable>() != null)
        {
            return Managers.ObjectPool.Pop(prefab, parent).gameObject;
        }


        GameObject go = Object.Instantiate(prefab, parent);
        go.name = prefab.name; // 뒤에 (clone) 붙는거 빼주기
        return go;
    }

    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        // 만약 오브젝트 풀이 있으면 거기다 넣는게 맞음
        Poolable data = go.GetComponent<Poolable>();
        if(data != null)
        {
            Managers.ObjectPool.Push(data);
            return;
        }

        Object.Destroy(go);
    }
}
