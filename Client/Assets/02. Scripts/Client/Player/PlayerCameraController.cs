using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCameraController : MonoBehaviour
{
    public float rotationSpeed = 1;
    public Transform cameraArm;
    public Transform pelvis;

    private bool amIPlayer;

    private float mouseX, mouseY;

    private void Awake()
    {
        amIPlayer = false;
    }

    private void Start()
    {
        if (amIPlayer == true) Cursor.lockState = CursorLockMode.Locked;
    }

    private void FixedUpdate()
    {
        if (amIPlayer == true)
        {
            Move();
            LookAround();
        }
    }

    private void Move()
    {
        cameraArm.position = new Vector3(pelvis.position.x, 0, pelvis.position.z);
    }
    private void LookAround()
    {
        Vector2 mouseDelta = new Vector2(Input.GetAxis("Mouse X") * rotationSpeed, Input.GetAxis("Mouse Y") * rotationSpeed);
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
    }
}
