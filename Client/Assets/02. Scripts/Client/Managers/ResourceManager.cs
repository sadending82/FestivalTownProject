using UnityEngine;

public class ResourceManager
{
    
    public T Load<T>(string path) where T : Object
    {
        // �ε��� �� ������Ʈ Ǯ���� �Ǵ� �� �� �Ǵ����� �ľ��ؼ� ���־� �ϴϱ� �̰ŵ� ���ľ� ��.

        // ������Ʈ Ǯ�� ������ �׳� ��������
        if (typeof(T) == typeof(GameObject))
        {
            string name = path;
            int index = name.LastIndexOf('/'); // "/"�� �ڿ��� �̸� ��������
            if (index >= 0)
                name = name.Substring(index + 1);

            GameObject go = Managers.ObjectPool.GetOrigin(name);
            if (go != null)
            {
                return go as T;
            }
        }

        // ������Ʈ Ǯ�� ������ �ε��ϱ�
        return Resources.Load<T>(path);
    }

    public Texture2D LoadTexture(string path)
    {
        Texture2D texture = Load<Texture2D>($"Textures/{path}");

        if(texture == null)
        {
            Debug.LogWarning($"{path}�� �ؽ��İ� ���ݾ�!");
            return null;
        }

        return texture;
    }

    public Sprite LoadSprite(string path)
    {
        Sprite sprite = Load<Sprite>($"Sprite/{path}");

        if (sprite == null)
        {
            Debug.LogWarning($"{path}�� ��������Ʈ�� �����...");
            return null;
        }

        return sprite;
    }

    public GameObject Instantiate(string path, Transform parent = null)
    {
        GameObject prefab = Load<GameObject>($"Prefabs/{path}");

        if(prefab == null)
        {
            Debug.LogWarning($"�ƴ� {path}�� ������ ���ݾƿ�;;");
            return null;
        }

        // ������Ʈ Ǯ�� ������ ���� ���� �ʿ� ����
        if (prefab.GetComponent<Poolable>() != null)
        {
            return Managers.ObjectPool.Pop(prefab, parent).gameObject;
        }


        GameObject go = Object.Instantiate(prefab, parent);
        go.name = prefab.name; // �ڿ� (clone) �ٴ°� ���ֱ�
        return go;
    }

    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        // ���� ������Ʈ Ǯ�� ������ �ű�� �ִ°� ����
        Poolable data = go.GetComponent<Poolable>();
        if(data != null)
        {
            Managers.ObjectPool.Push(data);
            return;
        }

        Object.Destroy(go);
    }
}
