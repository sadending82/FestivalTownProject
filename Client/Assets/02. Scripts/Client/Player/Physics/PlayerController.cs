using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class PlayerController : MonoBehaviour
{
    [Header("--- Physics ---")]
    public Transform cameraArm;
    public Transform stabilizer;
    public GameObject pelvis;
    private Rigidbody pelvisRigidbody;
    private Transform pelvisTransform;

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

    private AnimationController animationController;
    private float leftMouseClickTimer;
    private bool isHold;
    private bool isLeftShiftKeyDown;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;
    private ReceiveManager receiveManager;
    private float beforeAxisRawH, beforeAxisRawV;
    private float AxisRawH, AxisRawV;
    private bool amIPlayer;

    //------ Not Player -------
    private bool isMove;
    public float moveSpeed;
    private Vector3 moveDirection;

    private void Awake()
    {
        leftMouseClickTimer = 0f;
        isHold = false;
        isLeftShiftKeyDown = false;
        beforeAxisRawH = 0;
        beforeAxisRawV = 0;
    }
    void Start()
    {
        animationController = this.GetComponent<AnimationController>();
        network = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        pelvisRigidbody = pelvis.GetComponent<Rigidbody>();
        pelvisTransform = pelvis.GetComponent<Transform>();

        packetManager = network.GetPacketManager();
        receiveManager = network.GetReceiveManager();
    }

    private void FixedUpdate()
    {
        if (amIPlayer == true)
        {
            Move();
        }
        else
        {
            if (isMove == true)
            {
                GetComponent<Rigidbody>().velocity = moveDirection * moveSpeed;
            }
            if (moveDirection != Vector3.zero)
            {
                rotationQuaternion = Quaternion.LookRotation(moveDirection);
                stabilizer.rotation = rotationQuaternion;
            }
        }
    }
    private void Update()
    {
        if (amIPlayer == true)
        {
            MouseInput();
            CheckIsGround();
        }
    }

    private void CheckIsGround()
    {
        if(isGrounded == false && pelvisRigidbody.velocity.y <= 0)
        {
            // ray 그리기
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
        isLeftShiftKeyDown = false;
        if (Input.GetKey(KeyCode.LeftShift))
        {
            isLeftShiftKeyDown = true;
        }

        AxisRawH = Input.GetAxisRaw("Horizontal");
        AxisRawV = Input.GetAxisRaw("Vertical");
        Vector3 moveInput = new Vector3(AxisRawH, AxisRawV, 0);
        Vector3 lookForward = new Vector3(cameraArm.forward.x, 0f, cameraArm.forward.z).normalized;
        Vector3 lookRight = new Vector3(cameraArm.right.x, 0f, cameraArm.right.z).normalized;
        Vector3 moveDir = lookForward * moveInput.y + lookRight * moveInput.x;

        if (AxisRawH != beforeAxisRawH || AxisRawV != beforeAxisRawV)
        {
            if(AxisRawH == 0 && AxisRawV == 0)
            {
                packetManager.SendPlayerStopPacket(pelvisTransform.position, moveDir);
            }
            else
            {
                packetManager.SendPlayerMovePacket(pelvisTransform.position, moveDir);
            }
        }
        beforeAxisRawH = AxisRawH;
        beforeAxisRawV = AxisRawV;

        if (moveInput != Vector3.zero)
        {
            if (isLeftShiftKeyDown)
            {
                pelvisRigidbody.velocity = moveDir * runSpeed;
                if (isGrounded == true)
                {
                    animationController.setLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
            }
            else
            {
                pelvisRigidbody.velocity = moveDir * walkSpeed;
                if (isGrounded == true)
                {
                    animationController.setLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                }
            }
        }
        else 
        {
            if (isGrounded == true)
            {
                animationController.setLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
            }
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
                animationController.setLowerBodyAnimationState(LowerBodyAnimationState.JUMP);
                isGrounded = false;
            }
        }
    }
    private void MouseInput()
    {
        if (isLeftShiftKeyDown == true)
        {
            // 마우스 좌클릭 다운
            if (Input.GetMouseButtonDown(0))
            {
                if (isGrounded == true)
                {
                    animationController.setUpperBodyAnimationState(UpperBodyAnimationState.POWERATTACK);
                }
                else
                {
                    animationController.setLowerBodyAnimationState(LowerBodyAnimationState.FLYINGKICK);
                }
            }
        }
        else
        {
            // 마우스 좌클릭 다운
            if (Input.GetMouseButtonDown(0))
            {
                leftMouseClickTimer = 0f;
            }
            // 마우스 좌클릭 홀드
            if (Input.GetMouseButton(0))
            {
                leftMouseClickTimer += Time.deltaTime;
                if (leftMouseClickTimer >= 1f && isHold == false)
                {
                    // 잡기 애니메이션 작동
                    isHold = true;
                    animationController.setUpperBodyAnimationState(UpperBodyAnimationState.HOLD);
                }
            }
            // 마우스 좌클릭 업
            if (Input.GetMouseButtonUp(0))
            {
                if (isHold == true)
                {
                    isHold = false;
                    animationController.setUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                }
                else
                {
                    animationController.setUpperBodyAnimationState(UpperBodyAnimationState.ATTACK);
                }
                leftMouseClickTimer = 0f;
            }
        }

        // 마우스 휠클릭 다운
        if (Input.GetMouseButtonDown(2))
        {
            animationController.setUpperBodyAnimationState(UpperBodyAnimationState.HEADATTACK);
        }
        // 마우스 휠클릭 업
        if (Input.GetMouseButtonUp(2))
        {
            animationController.setUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        }

        // 마우스 우클릭 다운
        if (Input.GetMouseButtonDown(1))
        {
            animationController.setUpperBodyAnimationState(UpperBodyAnimationState.THROW);
        }
    }
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
    }
    public void SetPosition(Vector3 position)
    {
        this.transform.position = new Vector3(position.x, this.transform.position.y, position.z);
    }
    public void SetDirection(Vector3 direction)
    {
        moveDirection = direction;
    }
    public void SetIsMove(bool isMove)
    {
        this.isMove = isMove;
    }
}
