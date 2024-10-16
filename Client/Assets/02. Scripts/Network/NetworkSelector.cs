#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

public class NetworkSelect : EditorWindow
{
    [SerializeField]
    static private IPAddressData IPAddressObj;

    int _networkSelected;
    string _ipAddressText;

    [MenuItem("Network/NetworkSelect %#F5")]

    static void Open()
    {
        GetWindow<NetworkSelect>();
        
        IPAddressObj = Resources.Load<IPAddressData>("ScriptableObject/IPAddress");
    }

    private void OnGUI()
    {

        if (IPAddressObj == null)
        {
            IPAddressObj = Resources.Load<IPAddressData>("ScriptableObject/IPAddress");
        }

        GUIStyle labelStyle = EditorStyles.boldLabel;
        labelStyle.alignment = TextAnchor.MiddleCenter;
        GUILayout.Label("네트워크 설정", EditorStyles.boldLabel);

        _networkSelected = GUILayout.Toolbar(_networkSelected, new string[] { "로컬 네트워크로 설정", "리모트 네트워크로 설정", "테스트 서버로 설정", "IP 직접 입력" });

        if (_networkSelected == 3)
        {
            GUILayout.Space(10);
            GUILayout.Label("IP 주소");
            _ipAddressText = GUILayout.TextField(_ipAddressText);

        }

        switch (_networkSelected)
        {
            case 0:
                IPAddressObj.IPAddress = "127.0.0.1";
                break;
            case 1:
                IPAddressObj.IPAddress = "39.120.204.67";
                break;
            case 2:
                IPAddressObj.IPAddress = "203.239.231.148";
                break;
            case 3:
                IPAddressObj.IPAddress = _ipAddressText;
                break;
            default:
                break;
        }

        labelStyle.alignment = TextAnchor.MiddleLeft;

        minSize = new Vector3(600.0f, 200.0f);
        maxSize = new Vector3(600.0f, 200.0f);
    }
}
#endif
