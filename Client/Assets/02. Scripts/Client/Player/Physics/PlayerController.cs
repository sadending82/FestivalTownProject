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
//    private float fKeyDownTimer;
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
                pelvis.transform.position += moveDirection * walkSpeed * Time.deltaTime;
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
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerState.PS_JUMPSTOP);
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
                        packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerState.PS_JUMPSTOP);
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
                    packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerState.PS_MOVESTOP);
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
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerState.PS_RUN);
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
                    playerStatus.setLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
            }
            else
            {
                pelvis.transform.position += moveDirection * walkSpeed * Time.deltaTime;
                if (isGrounded == true && nowLowerBodyAnimationState != LowerBodyAnimationState.WALK)
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.WALK;
                    playerStatus.setLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                }
            }
        }
        else 
        {
            if (isGrounded == true && nowLowerBodyAnimationState != LowerBodyAnimationState.IDLE)
            {
                nowLowerBodyAnimationState = LowerBodyAnimationState.IDLE;
                playerStatus.setLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
            }
        }

        if (Input.GetAxis("Jump") > 0)
        {
            if(isGrounded == true)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerState.PS_JUMP);
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
        /// F 키를 눌렀을때 플레이어가 아이템을 가지지 않고 
        /// 주울 수 있는 범위 내에 아이템이 존재하면 픽업모드 시작
        /// </summary>
        if (Input.GetKeyDown(KeyCode.F) &&
            nearObjectChecker.GetNearObject() != null &&
            playerStatus.GetIsHaveItem() == false)
        {
            // fKeyDownTimer = 0;
            isPickUpMode = true;
            targetItem = nearObjectChecker.GetNearObject();

            if (targetItem.tag == "Bomb")
            {
                Bomb targetBomb = targetItem.GetComponent<Bomb>();
                Debug.Log("Target Bomb : " + targetBomb.GetId());
                packetManager.SendPlayerGrabBombPacket(pelvis.transform.position, stabillizerDirection, myId, targetBomb.GetId());
            }
        }
        if(Input.GetKey(KeyCode.F) && isPickUpMode == true)
        {
            // 목표 아이템이 사라졌거나 변경 됐으면 픽업모드 초기화
            if(nearObjectChecker.GetNearObject() == null || targetItem != nearObjectChecker.GetNearObject())
            {
//                fKeyDownTimer = 0;
                isPickUpMode = false;
            }
            ///<summary>
            /// 클라 테스트 용 서버에서는 필요없을듯 - 서버에서 잡아야하는지 여부를 보내주기 때문에
            /// </summary>
            //else if (fKeyDownTimer >= 1f && playerStatus.GetIsHaveItem() == false)
            //{
            //    PickUpItem();
            //}
            //fKeyDownTimer += Time.deltaTime;
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
            // 마우스 좌클릭 다운
            if (Input.GetMouseButtonDown(0))
            {
                if (isGrounded == true)
                {
                    playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.POWERATTACK);
                }
                else
                {
                    playerStatus.setLowerBodyAnimationState(LowerBodyAnimationState.FLYINGKICK);
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
                    playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.HOLD);
                }
            }
            // 마우스 좌클릭 업
            if (Input.GetMouseButtonUp(0))
            {
                if (isHold == true)
                {
                    isHold = false;
                    playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                }
                else
                {
                    playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.ATTACK);
                }
                leftMouseClickTimer = 0f;
            }
        }

        // 마우스 휠클릭 다운
        if (Input.GetMouseButtonDown(2))
        {
            playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.HEADATTACK);
        }
        // 마우스 휠클릭 업
        if (Input.GetMouseButtonUp(2))
        {
            playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        }

        // 마우스 우클릭 다운
        if (Input.GetMouseButtonDown(1))
        {
            playerStatus.setUpperBodyAnimationState(UpperBodyAnimationState.THROW);
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
                // 나중에 오브젝트 풀링 적용하면 폭탄 직접 자식객체로 넣었다가 돌려주도록 설정하자!!
                //-----------------------------------------------------------------------------------
                Bomb targetBomb = targetItem.GetComponent<Bomb>();
                targetBomb.PickUp(playerStatus.GetId(), bombInvenTransform);
                playerStatus.SetIsHaveItem(true);
            }
            else
            {
                Debug.Log("Error!! PickUpItem(), Target Item Tag is Wrong !!!");
            }
        }
    }
    public void s_PickUpItem(int playerId, int bombId)
    {
        // 폭탄과 플레이어 붙여주기
        Bomb targetBomb = GameObject.Find("BombObjectManager").transform.GetChild(bombId).GetComponent<Bomb>();
        targetBomb.PickUp(playerId, bombInvenTransform);
        playerStatus.SetIsHaveItem(true);
    }
    public void Jump()
    {
        isGrounded = false;
        pelvisRigidbody.velocity = Vector3.up * jumpForce;
        nowLowerBodyAnimationState = LowerBodyAnimationState.JUMP;
        playerStatus.setLowerBodyAnimationState(LowerBodyAnimationState.JUMP);
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

        pelvis.transform.position = new Vector3(position.x, pelvis.transform.position.y, position.z);
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
}
