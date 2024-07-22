using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    [Header("--- Physics ---")]
    public Transform cameraArm;
    public Transform stabilizer;
    public Rigidbody pelvisRigidbody;

    [Header("--- State ---")]
    public float walkSpeed;
    public float runSpeed;
    public float jumpForce;
    public float rotateSpeed;

    [Header("--- JumpCheck ---")]
    public float floorDetectionDistance;
    public Rigidbody leftFootRigidbody;
    public Rigidbody rightFootRigidbody;
    public bool isGrounded;

    private Quaternion rotationQuaternion;
    private void Awake()
    {
    }
    void Start()
    {
    }

    private void FixedUpdate()
    {
        Move();
    }
    private void Update()
    {
        CheckIsGround();
    }

    private void CheckIsGround()
    {
        if(isGrounded == false && pelvisRigidbody.velocity.y <= 0)
        {
            // ray ±×¸®±â
            //Debug.DrawRay(leftFootRigidbody.position, Vector3.down, new Color(0, 1, 0));
            //Debug.DrawRay(rightFootRigidbody.position, Vector3.down, new Color(0, 1, 0));

            Ray rayL = new Ray(leftFootRigidbody.position, Vector3.down);
            Ray rayR = new Ray(rightFootRigidbody.position, Vector3.down);

            RaycastHit hitInfoL, hitinfoR;
            if (Physics.Raycast(rayL, out hitInfoL, floorDetectionDistance) == true)
            {
                if (hitInfoL.collider.gameObject.tag == "Ground") isGrounded = true;
            }
            else if (Physics.Raycast(rayR, out hitinfoR, floorDetectionDistance) == true)
            {
                if (hitinfoR.collider.gameObject.tag == "Ground") isGrounded = true;
            }
        }
    }
    private void Move()
    {
        Vector2 moveInput = new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"));
        Vector3 lookForward = new Vector3(cameraArm.forward.x, 0f, cameraArm.forward.z).normalized;
        Vector3 lookRight = new Vector3(cameraArm.right.x, 0f, cameraArm.right.z).normalized;
        Vector3 moveDir = lookForward * moveInput.y + lookRight * moveInput.x;

        if (Input.GetKey(KeyCode.LeftShift))
        {
            pelvisRigidbody.velocity = moveDir * runSpeed;
        }
        else
        {
            pelvisRigidbody.velocity = moveDir * walkSpeed;
        }

        if (moveDir != Vector3.zero)
        {
            rotationQuaternion = Quaternion.LookRotation(moveDir);
            stabilizer.rotation = rotationQuaternion;
        }

        if (Input.GetAxis("Jump") > 0)
        {
            if(isGrounded == true)
            {
                pelvisRigidbody.velocity = Vector3.up * jumpForce;
                isGrounded = false;
            }
        }
    }
}
