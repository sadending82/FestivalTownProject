using System.Collections;
using System.Collections.Generic;
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
        // 카메라 보게 하기
        transform.rotation = cam.transform.rotation;
        transform.position = new Vector3(pelvis.transform.position.x, pelvis.transform.position.y + offsetY, pelvis.transform.position.z);

        // 카메라에 항상 보이게 하기
        // 만들어
    }
    public void SetCamera(Camera targetCamera)
    {
        cam = targetCamera.gameObject;
    }
}
