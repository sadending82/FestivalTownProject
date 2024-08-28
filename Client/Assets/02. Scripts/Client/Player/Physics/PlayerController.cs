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
    private CharacterStatus playerStatus;
    public float walkSpeed;
    public float runSpeed;
    public float jumpForce;
    public float rotateSpeed;
    private bool isMove;
    private Vector3 moveDirection;

    [Header("--- JumpCheck ---")]
    public float floorDetectionDistance;
    public Rigidbody leftFootRigidbody;
    public Rigidbody rightFootRigidbody;
    public bool isGrounded;

    private Quaternion rotationQuaternion;
    private Vector3 stabillizerDirection;

    [Header("--- Animation ---")]
    private float leftMouseClickTimer;
    private bool isHold;
    private bool isLeftShiftKeyDown;
    private LowerBodyAnimationState nowLowerBodyAnimationState;

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

    [Header("--- Pick Up ---")]
    public NearObjectChecker nearObjectChecker;
    public Transform bombInvenTransform;
    private float fKeyDownTimer;
    private bool isPickUpMode = false;
    private GameObject targetItem;

    private void Awake()
    {
        leftMouseClickTimer = 0f;
        isHold = false;
        isLeftShiftKeyDown = false;
        beforeAxisRawH = 0;
        beforeAxisRawV = 0;
        amIPlayer = false;
        SetIsMove(false);
        stabillizerDirection = Vector3.zero;
        nowLowerBodyAnimationState = LowerBodyAnimationState.IDLE;
    }
    void Start()
    {
        network = Managers.Network;
        pelvisRigidbody = pelvis.GetComponent<Rigidbody>();
        playerStatus = this.GetComponent<CharacterStatus>();

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
            if (stabillizerDirection != moveDirection && moveDirection != Vector3.zero)
            {
                stabillizerDirection = moveDirection;
                rotationQuaternion = Quaternion.LookRotation(moveDirection);
                stabilizer.rotation = rotationQuaternion;
            }
            if (isMove == true)
            {
                if (playerStatus.GetLowerBodyAnimationState() == LowerBodyAnimationState.WALK)
                {
                    pelvis.transform.position += moveDirection * walkSpeed * Time.deltaTime;
                }
                else if(playerStatus.GetLowerBodyAnimationState() == LowerBodyAnimationState.RUN)
                {
                    pelvis.transform.position += moveDirection * runSpeed * Time.deltaTime;
                }
            }
        }
    }
    private void Update()
    {
        if (amIPlayer == true)
        {
            KeyboardInput();
            if (isPickUpMode == false)
            {
                MouseInput();
            }
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
            packetManager.SendPlayerPosPacket(pelvis.transform.position, stabillizerDirection, myId);
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
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_JUMPSTOP);
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
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_JUMPSTOP);
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

        if(stabillizerDirection != moveDirection && moveDirection != Vector3.zero)
        {
            stabillizerDirection = moveDirection;
            rotationQuaternion = Quaternion.LookRotation(moveDirection);
            stabilizer.rotation = rotationQuaternion;
        }

        if (AxisRawH != beforeAxisRawH || AxisRawV != beforeAxisRawV)
        {
            if (moveInput == Vector3.zero)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_MOVESTOP);
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
                    if (isLeftShiftKeyDown == true)
                    {
                        packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_RUN);
                    }
                    else
                    {
                        packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_WALK);
                    }
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
                if (isGrounded == true && nowLowerBodyAnimationState != LowerBodyAnimationState.RUN)
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.RUN;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
            }
            else
            {
                pelvis.transform.position += moveDirection * walkSpeed * Time.deltaTime;
                if (isGrounded == true && nowLowerBodyAnimationState != LowerBodyAnimationState.WALK)
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.WALK;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                }
            }
        }
        else
        {
            if (nowLowerBodyAnimationState != LowerBodyAnimationState.IDLE)
            {
                nowLowerBodyAnimationState = LowerBodyAnimationState.IDLE;
                playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
            }
        }

        if (Input.GetAxis("Jump") > 0)
        {
            if(isGrounded == true)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_JUMP);
                }
                else
                {
                    Debug.Log("Not Send Jump Packet, Pelvis is Null !!!");
                }
                Jump();
            }
        }
    }
    private void KeyboardInput()
    {
        /// <summary>
        /// F Ű�� �������� �÷��̾ �������� ������ �ʰ� 
        /// �ֿ� �� �ִ� ���� ���� �������� �����ϸ� �Ⱦ���� ����
        /// </summary>
        if (Input.GetKeyDown(KeyCode.F) &&
            nearObjectChecker.GetNearObject() != null &&
            playerStatus.GetIsHaveItem() == false)
        {
            fKeyDownTimer = 0;
            isPickUpMode = true;
            targetItem = nearObjectChecker.GetNearObject();
        }
        if(Input.GetKey(KeyCode.F) && isPickUpMode == true)
        {
            fKeyDownTimer += Time.deltaTime;
            // ��ǥ �������� ������ų� ���� ������ �Ⱦ���� �ʱ�ȭ
            if(nearObjectChecker.GetNearObject() == null || targetItem != nearObjectChecker.GetNearObject())
            {
                fKeyDownTimer = 0;
                isPickUpMode = false;
            }
            else if (fKeyDownTimer >= 1f && playerStatus.GetIsHaveItem() == false)
            {
                if (targetItem.tag == "Bomb")
                {
                    Bomb targetBomb = targetItem.GetComponent<Bomb>();
                    packetManager.SendPlayerGrabBombPacket(pelvis.transform.position, stabillizerDirection, myId, targetBomb.GetId());
                }
                // Ŭ�� �׽�Ʈ��
                //PickUpItem();
            }
        }
        if(Input.GetKeyUp(KeyCode.F) && isPickUpMode == true)
        {
            isPickUpMode = false;
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
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.POWERATTACK);
                }
                else
                {
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.FLYINGKICK);
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
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.HOLD);
                }
            }
            // ���콺 ��Ŭ�� ��
            if (Input.GetMouseButtonUp(0))
            {
                if (isHold == true)
                {
                    isHold = false;
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                }
                else
                {
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.ATTACK);
                }
                leftMouseClickTimer = 0f;
            }
        }

        // ���콺 ��Ŭ�� �ٿ�
        if (Input.GetMouseButtonDown(2))
        {
            playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.HEADATTACK);
        }
        // ���콺 ��Ŭ�� ��
        if (Input.GetMouseButtonUp(2))
        {
            playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        }

        // ���콺 ��Ŭ�� �ٿ�
        if (Input.GetMouseButtonDown(1))
        {
            playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.THROW);
            Throw();
        }
    }
    private void PickUpItem()
    {
        if (targetItem == null)
        {
            Debug.Log("Error!! PickUpItem(), You don't Have Target Item !!!");
        }
        else
        {
            if (targetItem.tag == "Bomb")
            {
                //-----------------------------------------------------------------------------------
                // ���߿� ������Ʈ Ǯ�� �����ϸ� ��ź ���� �ڽİ�ü�� �־��ٰ� �����ֵ��� ��������!!
                //-----------------------------------------------------------------------------------
                Bomb targetBomb = targetItem.GetComponent<Bomb>();
                targetBomb.PickUp(playerStatus.GetId(), bombInvenTransform);
                playerStatus.SetIsHaveItem(true, "Bomb", targetBomb.GetId());
            }
            else
            {
                Debug.Log("Error!! PickUpItem(), Target Item Tag is Wrong !!!");
            }
        }
    }
    /// <summary>
    /// �̰͵� ���� �Ŵ������� �����ؾ����� ������???
    /// </summary>
    public void s_PickUpItem(int playerId, int bombId)
    {
        // ��ź�� �÷��̾� �ٿ��ֱ�
        Bomb targetBomb = Managers.BombObject.FindBombById(bombId).GetComponent<Bomb>();
        targetBomb.PickUp(playerId, bombInvenTransform);
        playerStatus.SetIsHaveItem(true, "Bomb", bombId);
    }
    public void Jump()
    {
        isGrounded = false;
        pelvisRigidbody.velocity = Vector3.up * jumpForce;
    }

    public void Throw()
    {
        if(playerStatus.GetIsHaveItem() == true)
        {
            if(playerStatus.GetItemTag() == "Bomb")
            {
                GameObject targetBomb = Managers.BombObject.FindBombById(playerStatus.GetItemId());
                targetBomb.GetComponent<Bomb>().Throw(stabillizerDirection, playerStatus.GetStrength());

                // ������ �÷��̾� ��ġ, ��ź �߻� ����, ��ź ��ġ, �÷��̾� ���̵�, ��ź ���̵� ������
                packetManager.SendPlayerThrowBombPacket(targetBomb.transform.position, stabillizerDirection, myId, targetBomb.GetComponent<Bomb>().GetId());
                playerStatus.SetIsHaveItem(false);

                playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
            }
        }
        else
        {
            Debug.Log("Player " + playerStatus.GetId() + " Don't Have an Item to Throw !!!");
        }
    }
    public void s_Throw(Vector3 bombPosition, int bombId)
    {
        if (playerStatus.GetIsHaveItem() == true)
        {
            if (playerStatus.GetItemTag() == "Bomb")
            {
                GameObject targetBomb = Managers.BombObject.FindBombById(bombId);
                targetBomb.transform.position = bombPosition;
                targetBomb.GetComponent<Bomb>().Throw(moveDirection, playerStatus.GetStrength());

                playerStatus.SetIsHaveItem(false);
            }
        }
        else
        {
            Debug.Log("Player " + playerStatus.GetId() + " Don't Have an Item to Throw !!!");
        }
    }
    
    /// <summary> --------------------------------------
    /// �̰� ���� ���� �Ŵ����� ���߿� �� �Ű���...�Ф�
    /// </summary> -------------------------------------
    public void SendToServerGoalTeamNumber(int bombId, int teamNumber)
    {
        if(amIPlayer == true)
        {
            packetManager.SendBombInputPacket(bombId, teamNumber);
        }
    }

    // ------- Setter Getter -------
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
    }
    public void SetPosition(Vector3 position)
    {
        if (pelvis == null) {
            Debug.Log("pelvis Null!!!!");

            return; }

        pelvis.transform.position = position;
    }
    public void SetDirection(Vector3 direction)
    {
        moveDirection = direction;
    }
    public void SetIsMove(bool isMove)
    {
        this.isMove = isMove;
    }
    public void SetMyId(int myId)
    {
        this.myId = myId;
    }
    public void s_SetAnimation(ePlayerMoveState playerMoveState)
    {
        switch (playerMoveState)
        {
            case ePlayerMoveState.PS_WALK:
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.WALK;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                }
                break;
            case ePlayerMoveState.PS_RUN:
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.RUN;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
                break;
            default:
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.IDLE;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
                }
                break;
        }
    }
}
