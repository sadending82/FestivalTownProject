using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class PlayerUIController : MonoBehaviour
{
    private GameObject cam;
    private Vector3 startScale;
    private float distance;

    public CharacterStatus playerState;
    private GameObject pelvis;
    private float offsetY = 1.7f;

    private GameObject guideUI;
    private GameObject gageUI;

    private bool isGageOn = false;
    private float nowGage;
    private float maxGage;

    private void Start()
    {
        cam = playerState.GetMyCamera().gameObject;
        startScale = transform.localScale;
        distance = Vector3.Distance(cam.transform.position, transform.position);
        pelvis = playerState.GetPelvis();
        guideUI = transform.GetChild(5).gameObject;
        gageUI = guideUI.transform.GetChild(3).GetChild(0).GetChild(0).gameObject;
    }
    private void Update()
    {
        // ī�޶� ���� �ϱ�
        transform.rotation = cam.transform.rotation;
        transform.position = new Vector3(pelvis.transform.position.x, pelvis.transform.position.y + offsetY, pelvis.transform.position.z);

        // ī�޶� �׻� ���̰� �ϱ�
        // �����
    }
    private void FixedUpdate()
    {
        if (isGageOn == true)
        {
            nowGage += Time.deltaTime;
            gageUI.GetComponent<Image>().fillAmount = nowGage / maxGage;
            if (nowGage >= maxGage)
            {
                UIAllOff();
            }
        }
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

    public void UIDropOn(float maxGage)
    {
        this.maxGage = maxGage;
        guideUI.transform.GetChild(0).gameObject.SetActive(false);
        guideUI.transform.GetChild(1).gameObject.SetActive(false);
        guideUI.transform.GetChild(2).gameObject.SetActive(true);
        gageUI.gameObject.SetActive(true);
        isGageOn = true;
        nowGage = 0.0f;
    }
    public void UIDropOff()
    {
        guideUI.transform.GetChild(2).gameObject.SetActive(false);
        GageOff();
    }

    public void UIPickUpOn(float maxGage)
    {
        this.maxGage = maxGage;
        gageUI.gameObject.SetActive(true);
        isGageOn = true;
        nowGage = 0.0f;
    }
    public void UIPickUpOff()
    {
        guideUI.transform.GetChild(0).gameObject.SetActive(false);
        guideUI.transform.GetChild(1).gameObject.SetActive(false);
        GageOff();
    }

    public void GageOff()
    {
        gageUI.gameObject.SetActive(false);
        isGageOn = false;
        nowGage = 0.0f;
        maxGage = 0.0f;
    }
    private void UIAllOff()
    {
        guideUI.transform.GetChild(0).gameObject.SetActive(false);
        guideUI.transform.GetChild(1).gameObject.SetActive(false);
        guideUI.transform.GetChild(2).gameObject.SetActive(false);
        GageOff();
    }
}
