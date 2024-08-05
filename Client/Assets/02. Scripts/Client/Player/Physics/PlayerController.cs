using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;
using NetworkProtocol;

public class PlayerController : MonoBehaviour
{
    [Header("--- Physics ---")]
    public Transform cameraArm;
    public Transform stabilizer;
    public GameObject pelvis;
    private Rigidbody pelvisRigidbody;

    [Header("--- State ---")]
    public float walkSpeed;
    public float runSpeed;
    public float jumpForce;
    public float rotateSpeed;
    private bool isMove;
    private Vector3 moveDirection;
    private Vector3 sMoveDirection;

    [Header("--- JumpCheck ---")]
    public float floorDetectionDistance;
    public Rigidbody leftFootRigidbody;
    public Rigidbody rightFootRigidbody;
    public bool isGrounded;

    private Quaternion rotationQuaternion;

    [Header("--- Animation ---")]
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
    private int myId;

    //------ timer for send ------
    private float curTime= 0.0f;
    private float sendInterval = 0.01666666666f;


    private void Awake()
    {
        leftMouseClickTimer = 0f;
        isHold = false;
        isLeftShiftKeyDown = false;
        beforeAxisRawH = 0;
        beforeAxisRawV = 0;
        amIPlayer = false;
        SetIsMove(false);
    }
    void Start()
    {
        animationController = this.GetComponent<AnimationController>();
        network = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        pelvisRigidbody = pelvis.GetComponent<Rigidbody>();

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
                pelvis.transform.position += sMoveDirection * walkSpeed * Time.deltaTime;
            }
            rotationQuaternion = Quaternion.LookRotation(sMoveDirection);
            stabilizer.rotation = rotationQuaternion;
        }
    }
    private void Update()
    {
        if (amIPlayer == true)
        {
            MouseInput();
            if (gameObject != null)
            {
                curTime += Time.deltaTime;
                if (curTime > sendInterval)
                {
                    curTime -= sendInterval;
                    SendForSync();
                }
            }
        }
        
        CheckIsGround();
    }

    private void SendForSync()
    {
        if (pelvis != null)
        {
            packetManager.SendPlayerPosPacket(pelvis.transform.position, sMoveDirection, myId);
        }
    }

    private void CheckIsGround()
    {
        if(isGrounded == false && pelvisRigidbody.velocity.y <= 0)
        {
            Ray rayL = new Ray(leftFootRigidbody.position, Vector3.down);
            Ray rayR = new Ray(rightFootRigidbody.position, Vector3.down);

            RaycastHit hitInfoL, hitinfoR;
            if (Physics.Raycast(rayL, out hitInfoL, floorDetectionDistance) == true)
            {
                if (hitInfoL.collider.gameObject.tag == "Ground")
                {
                    if (pelvis != null)
                    {
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, moveDirection, myId, ePlayerState.PS_JUMPSTOP);
                    }
                    else
                    {
                        Debug.Log("Not Send Jump Stop Packet, Pelvis is Null !!!");
                    }
                    isGrounded = true;
                }
            }
            else if (Physics.Raycast(rayR, out hitinfoR, floorDetectionDistance) == true)
            {
                if (hitinfoR.collider.gameObject.tag == "Ground")
                {
                    if (pelvis != null)
                    {
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, moveDirection, myId, ePlayerState.PS_JUMPSTOP);
                    }
                    else
                    {
                        Debug.Log("Not Send Jump Stop Packet, Pelvis is Null !!!");
                    }
                    isGrounded = true;
                }
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
        moveDirection = lookForward * moveInput.y + lookRight * moveInput.x;
        moveDirection = moveDirection.normalized;

        if (AxisRawH != beforeAxisRawH || AxisRawV != beforeAxisRawV)
        {
            if (moveInput == Vector3.zero)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerStopPacket(pelvis.transform.position, moveDirection, myId, ePlayerState.PS_MOVESTOP);
                }
                else
                {
                    Debug.Log("Not Send Stop Packet, Pelvis is Null !!!");
                }
            }
            else
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, moveDirection, myId, ePlayerState.PS_RUN);
                }
                else
                {
                    Debug.Log("Not Send Run Packet, Pelvis is Null !!!");
                }
            }
        }
        beforeAxisRawH = AxisRawH;
        beforeAxisRawV = AxisRawV;

        if (moveInput != Vector3.zero)
        {
            if (isLeftShiftKeyDown)
            {
                pelvis.transform.position += moveDirection * runSpeed * Time.deltaTime;
                if (isGrounded == true)
                {
                    animationController.setLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
            }
            else
            {
                pelvis.transform.position += moveDirection * walkSpeed * Time.deltaTime;
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

        if (moveDirection != Vector3.zero)
        {
            rotationQuaternion = Quaternion.LookRotation(moveDirection);
            stabilizer.rotation = rotationQuaternion;
        }

        if (Input.GetAxis("Jump") > 0)
        {
            if(isGrounded == true)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, moveDirection, myId, ePlayerState.PS_JUMP);
                }
                else
                {
                    Debug.Log("Not Send Jump Packet, Pelvis is Null !!!");
                }
                Jump();
            }
        }
    }
    private void MouseInput()
    {
        if (isLeftShiftKeyDown == true)
        {
            // ���콺 ��Ŭ�� �ٿ�
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
            // ���콺 ��Ŭ�� �ٿ�
            if (Input.GetMouseButtonDown(0))
            {
                leftMouseClickTimer = 0f;
            }
            // ���콺 ��Ŭ�� Ȧ��
            if (Input.GetMouseButton(0))
            {
                leftMouseClickTimer += Time.deltaTime;
                if (leftMouseClickTimer >= 1f && isHold == false)
                {
                    // ��� �ִϸ��̼� �۵�
                    isHold = true;
                    animationController.setUpperBodyAnimationState(UpperBodyAnimationState.HOLD);
                }
            }
            // ���콺 ��Ŭ�� ��
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

        // ���콺 ��Ŭ�� �ٿ�
        if (Input.GetMouseButtonDown(2))
        {
            animationController.setUpperBodyAnimationState(UpperBodyAnimationState.HEADATTACK);
        }
        // ���콺 ��Ŭ�� ��
        if (Input.GetMouseButtonUp(2))
        {
            animationController.setUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        }

        // ���콺 ��Ŭ�� �ٿ�
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
        Debug.Log(gameObject.name + "Set Position");
        if (pelvis == null) {
            Debug.Log("pelvis Null!!!!");

            return; }

        pelvis.transform.position = new Vector3(position.x, pelvis.transform.position.y, position.z);
    }
    public void SetDirection(Vector3 direction)
    {
        sMoveDirection = direction;
    }
    public void SetIsMove(bool isMove)
    {
        this.isMove = isMove;
    }
    public void Jump()
    {
        pelvisRigidbody.velocity = Vector3.up * jumpForce;
        Debug.Log(gameObject.name + " is Jump vy : " + pelvisRigidbody.velocity.y);
        animationController.setLowerBodyAnimationState(LowerBodyAnimationState.JUMP);
        isGrounded = false;
    }
    public void SetMyId(int myId)
    {
        this.myId = myId;
    }
}
