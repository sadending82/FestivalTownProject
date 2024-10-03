using PacketTable.GameTable;
using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class PlayerCameraController : MonoBehaviour
{
    private Camera myCamera;
    private Transform cameraArm;
    public Transform pelvis;

    [Header("--- Camera Controll ---")]
    public float RotationSpeed;
    public float ZoomSpeed;
    public float MinimumZoom;
    public float MaximumZoom;

    private bool amIPlayer;
    private int myId;
    private bool IsMainCamera;

    private float mouseX, mouseY;

    private void Awake()
    {
        myCamera = GetComponent<Camera>();
        cameraArm = transform.parent;
        amIPlayer = false;
        IsMainCamera = false;
    }

    private void Start()
    {
        if (amIPlayer == true)
        {
            Cursor.lockState = CursorLockMode.Locked;
        }
    }

    private void FixedUpdate()
    {
        if (IsMainCamera == true)
        {
            Move();
            LookAround();
        }
    }
    private void Update()
    {
        if (IsMainCamera == true)
        {
            float scroll = Input.GetAxis("Mouse ScrollWheel") * ZoomSpeed;

            myCamera.fieldOfView -= scroll;
        
            if (myCamera.fieldOfView < MinimumZoom)
            {
                myCamera.fieldOfView = MinimumZoom;
            }
            else if (myCamera.fieldOfView > MaximumZoom)
            {
                myCamera.fieldOfView = MaximumZoom;
            }
        }
    }
    private void LateUpdate()
    {
        if (IsMainCamera == true)
        {
            FindObjectBetweenCameraAndPlayer();
        }
    }
    private void Move()
    {
        cameraArm.position = new Vector3(pelvis.position.x, 0, pelvis.position.z);
    }
    private void LookAround()
    {
        Vector2 mouseDelta = new Vector2(Input.GetAxis("Mouse X") * RotationSpeed, Input.GetAxis("Mouse Y") * RotationSpeed);
        Vector3 cameraAngle = cameraArm.rotation.eulerAngles;
        float x = cameraAngle.x - mouseDelta.y;

       if (x < 180f)
       {
           x = Mathf.Clamp(x, -1f, 20f);
       }
       else
       {
           x = Mathf.Clamp(x, 320f, 361f);
       }
       cameraArm.rotation = Quaternion.Euler(x, cameraAngle.y + mouseDelta.x, cameraAngle.z);
    }

    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
        IsMainCamera = amIPlayer;
    }
    public bool GetAmIPlayer()
    {
        return amIPlayer;
    }
    public void SetMyId(int myId)
    {
        this.myId = myId;
    }
    public void SpectatorModeOn()
    {
        IsMainCamera = true;
    }
    public void SpectatorModeOff()
    {
        IsMainCamera = false;
    }
    public bool GetIsPlayerDie()
    {
        return Managers.Player.FindPlayerById(myId).GetComponent<CharacterStatus>().GetIsDie();
    }
    private void FindObjectBetweenCameraAndPlayer()
    {
        Vector3 direction = (pelvis.position - transform.position).normalized;
        float distance = Vector3.Distance(pelvis.position, transform.position);
        RaycastHit[] hits = Physics.RaycastAll(transform.position, direction, distance,
            LayerMask.GetMask("Cube", "Statue"));

        for (int i = 0; i < hits.Length; ++i)
        {
            TransparentObject[] obj = hits[i].transform.GetComponentsInChildren<TransparentObject>();
            for (int j = 0; j < obj.Length; ++j)
            {
                obj[j]?.BecomeTransparent();
            }
        }
    }
}