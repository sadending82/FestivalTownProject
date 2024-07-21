using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public float walkSpeed;
    public float runSpeed;
    public float jumpForce;
    public float rotateSpeed;
    public Transform cameraArm;
    public Transform stabilizer;

    public Rigidbody pelvisRigidbody;
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
    }

    private void Move()
    {
        Vector2 moveInput = new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"));
        Vector3 lookForward = new Vector3(cameraArm.forward.x, 0f, cameraArm.forward.z).normalized;
        Vector3 lookRight = new Vector3(cameraArm.right.x, 0f, cameraArm.right.z).normalized;
        Vector3 moveDir = lookForward * moveInput.y + lookRight * moveInput.x;

        // �޸��ٸ�
        if (Input.GetKey(KeyCode.LeftShift))
        {
            pelvisRigidbody.velocity = moveDir * runSpeed;
        }
        // �ȴ´ٸ�
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
                /*
                �� �߷� �׶������� üũ�ϱ� ������ �� ���� ������ ���� ���� ������� AddForce�� �� �� �ϴ� ���� �����ϴ� ���� �߰��ؾ���
                */
                pelvisRigidbody.AddForce(new Vector3(0, jumpForce, 0), ForceMode.Impulse);
                isGrounded = false;
            }
        }
    }
}
