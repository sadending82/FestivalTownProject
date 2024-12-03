using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class PlayerUIController : MonoBehaviour
{
    private GameObject cam;
    private Vector3 startScale;
    private float distance;

    public CharacterStatus playerState;
    private GameObject pelvis;
    private float offsetY = 1.7f;

    private void Start()
    {
        cam = playerState.GetMyCamera().gameObject;
        startScale = transform.localScale;
        distance = Vector3.Distance(cam.transform.position, transform.position);
        pelvis = playerState.GetPelvis();
    }
    private void Update()
    {
        // ī�޶� ���� �ϱ�
        transform.rotation = cam.transform.rotation;
        transform.position = new Vector3(pelvis.transform.position.x, pelvis.transform.position.y + offsetY, pelvis.transform.position.z);

        // ī�޶� �׻� ���̰� �ϱ�
        // �����
    }
    public void SetCamera(Camera targetCamera)
    {
        cam = targetCamera.gameObject;
    }
    public void SetNickName(string nickName)
    {
        switch(playerState.GetTeamNumber())
        {
            // ������ ���� �ȵ����� �ӽ������� �ϴ� �̷��� �صα�.
            case 0:
                transform.GetChild(1).GetChild(0).GetChild(1).GetChild(0).GetComponent<TMP_Text>().text = nickName;
                break;
            case 1:
                transform.GetChild(2).GetChild(0).GetChild(1).GetChild(0).GetComponent<TMP_Text>().text = nickName;
                break;
            case 2:
                transform.GetChild(3).GetChild(0).GetChild(1).GetChild(0).GetComponent<TMP_Text>().text = nickName;
                break;
        }
    }
}
