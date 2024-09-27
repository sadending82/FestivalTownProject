using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ClientProtocol;
using NetworkProtocol;
using UnityEngine.Rendering;
using TMPro.Examples;
using static UnityEngine.GraphicsBuffer;

public class PlayerController : MonoBehaviour
{
    [Header("--- Physics ---")]
    public Transform cameraArm;
    public Transform stabilizer;
    public GameObject pelvis;
    private Rigidbody pelvisRigidbody;
    public GameObject clavicleL;
    public GameObject clavicleR;
    public GameObject head;
    public GameObject handL;
    public GameObject handR;

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
    public bool isJump;

    private Quaternion rotationQuaternion;
    private Vector3 stabillizerDirection;

    [Header("--- Animation ---")]
    private float leftMouseClickTimer;
    private bool isGrap;
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
    private bool gameStart = false;

    //------ timer for send ------
    private float curTime= 0.0f;
    private float sendInterval = 0.01666666666f * 3;

    [Header("--- Pick Up ---")]
    public NearObjectChecker nearObjectChecker;
    public Transform bombInvenTransform;
    private float fKeyDownTimer;
    private bool isPickUpMode = false;
    private GameObject targetItem;
    private bool isDropMode = false;
    private bool isGrapPlayerMode = false;

    [Header("--- Respawn ---")]
    private float createHeight = 4;

    //Test
    private int c = 0;

    private void Awake()
    {
        ResetPlayerControllerSetting();
        amIPlayer = false;
        stabillizerDirection = Vector3.zero;

        network = Managers.Network;
        pelvisRigidbody = pelvis.GetComponent<Rigidbody>();
        playerStatus = this.GetComponent<CharacterStatus>();
        myId = playerStatus.GetId();

        packetManager = network.GetPacketManager();
        receiveManager = network.GetReceiveManager();
    }
    void Start()
    {
    }

    private void FixedUpdate()
    {
        if (playerStatus.GetIsDie() == false)
        {
            FallDownCheck();
            // ����ְ� �׷α� ���°� �ƴ�
            if (playerStatus.GetIsGroggy() == false)
            {
                if (amIPlayer == true && gameStart == true)
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
                    if (isMove == true && CheckHitWall() == false)
                    {
                        if (playerStatus.GetLowerBodyAnimationState() == LowerBodyAnimationState.WALK)
                        {
                            if (isGrounded == false)
                            {
                                pelvisRigidbody.velocity = (moveDirection * (walkSpeed - 2));
                            }
                            else
                            {
                                pelvisRigidbody.velocity = moveDirection * walkSpeed;
                            }
                        }
                        else if (playerStatus.GetLowerBodyAnimationState() == LowerBodyAnimationState.RUN)
                        {
                            if (isGrounded == false)
                            {
                                pelvisRigidbody.velocity = moveDirection * (runSpeed - 3);
                            }
                            else
                            {
                                pelvisRigidbody.velocity = moveDirection * runSpeed;
                            }
                        }

                        if (isJump == true)
                        {
                            if (isGrounded == true)
                            {
                                Jump();
                                isJump = false;
                            }
                        }
                    }
                }
            }
        }
    }
    private void Update()
    {
        // �׽�Ʈ
        if (amIPlayer == true)
        {
            if (Input.GetKeyUp(KeyCode.T))
            {
                Managers.CubeObject.SpawnCube(c, c, 0);
                c++;
                c %= 10;
            }
        }

        if (playerStatus.GetIsDie() == false)
        {
            if (amIPlayer == true && gameStart == true)
            {
                if (playerStatus.GetIsGroggy() == false)
                {
                    KeyboardInput();
                    if (isPickUpMode == false)
                    {
                        MouseInput();
                    }
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
    }

    private void SendForSync()
    {
        if (pelvis != null && gameStart == true && playerStatus.GetIsGroggy() == false)
        {
            packetManager.SendPlayerSyncPacket(pelvis.transform.position, stabillizerDirection, playerStatus.GetStamina(), myId);
        }
    }

    private void CheckIsGround()
    {
        if(isGrounded == false && pelvisRigidbody.velocity.y <= 0)
        {
            Ray rayL = new Ray(leftFootRigidbody.position, Vector3.down);
            Ray rayR = new Ray(rightFootRigidbody.position, Vector3.down);

            AxisRawH = Input.GetAxisRaw("Horizontal");
            AxisRawV = Input.GetAxisRaw("Vertical");
            Vector3 moveInput = new Vector3(AxisRawH, AxisRawV, 0);

            RaycastHit hitInfoL, hitinfoR;
            if (Physics.Raycast(rayL, out hitInfoL, floorDetectionDistance) == true)
            {
                if (hitInfoL.collider.gameObject.tag == "Ground")
                {
                    if (pelvis != null && amIPlayer == true)
                    {
                        if (moveInput == Vector3.zero)
                        {
                            if (pelvis != null)
                            {
                                packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_JUMPSTOP);
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
                    isGrounded = true;
                }
            }
            else if (Physics.Raycast(rayR, out hitinfoR, floorDetectionDistance) == true)
            {
                if (hitinfoR.collider.gameObject.tag == "Ground")
                {
                    if (pelvis != null && amIPlayer == true)
                    {
                        if (moveInput == Vector3.zero)
                        {
                            if (pelvis != null)
                            {
                                packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_JUMPSTOP);
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
                if (CheckHitWall() == false)
                {
                    if (isGrounded == false)
                    {
                        pelvisRigidbody.velocity = moveDirection * (runSpeed - 3);
                    }
                    else
                    {
                        pelvisRigidbody.velocity = moveDirection * runSpeed;
                    }
                }
                if (isGrounded == true && nowLowerBodyAnimationState != LowerBodyAnimationState.RUN)
                {
                    nowLowerBodyAnimationState = LowerBodyAnimationState.RUN;
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
            }
            else
            {
                if (CheckHitWall() == false)
                {
                    if (isGrounded == false)
                    {
                        pelvisRigidbody.velocity = moveDirection * (walkSpeed - 2);
                    }
                    else
                    {
                        pelvisRigidbody.velocity = moveDirection * walkSpeed;
                    }
                }
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
                    Managers.Sound.Play3D("Sfx_Ch_Jump", gameObject);
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
        if (Input.GetKeyDown(KeyCode.F))
        {
            if (nearObjectChecker.GetNearObject() != null &&
                ((nearObjectChecker.GetNearObject().tag == "Bomb" && playerStatus.GetIsHaveBomb() == false) ||
                (nearObjectChecker.GetNearObject().tag == "Weapon" && playerStatus.GetIsHaveWeapon() == false)))
            {
                fKeyDownTimer = 0;
                isPickUpMode = true;
                targetItem = nearObjectChecker.GetNearObject();
            }
            else if (playerStatus.GetIsHaveWeapon() == true && isPickUpMode == false)
            {
                fKeyDownTimer = 0;
                isDropMode = true;
            }
        }
        if (Input.GetKeyUp(KeyCode.F))
        {
            isPickUpMode = false;
            isDropMode = false;
            fKeyDownTimer = 0;
        }
        if (Input.GetKey(KeyCode.F))
        {
            if (isPickUpMode == true)
            {
                fKeyDownTimer += Time.deltaTime;
                // ��ǥ �������� ������ų� ���� ������ �Ⱦ���� �ʱ�ȭ
                if (nearObjectChecker.GetNearObject() == null || targetItem != nearObjectChecker.GetNearObject())
                {
                    fKeyDownTimer = 0;
                    isPickUpMode = false;
                }
                else if (fKeyDownTimer >= 1f)
                {
                    if (targetItem.tag == "Bomb" && playerStatus.GetIsHaveBomb() == false)
                    {
                        Bomb targetBomb = targetItem.GetComponent<Bomb>();
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerGrabBombPacket(pelvis.transform.position, stabillizerDirection, myId, targetBomb.GetId());
                    }
                    else if (targetItem.tag == "Weapon" && playerStatus.GetIsHaveWeapon() == false)
                    {
                        Weapon targetWeapon = targetItem.GetComponent<Weapon>();
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerGrabWeaponPacket(pelvis.transform.position, stabillizerDirection, myId, targetWeapon.GetId());
                    }
                }
            }
            else if (isDropMode == true)
            {
                fKeyDownTimer += Time.deltaTime;
                if (fKeyDownTimer >= 1f)
                {
                    if (playerStatus.GetIsHaveWeapon() == true)
                    {
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerDropWeaponPacket(GetPosition(), playerStatus.GetWeaponId());
                    }
                }
            }
        }
    }
    private void MouseInput()
    {
        if (isLeftShiftKeyDown == true)
        {
            if (playerStatus.GetUpperBodyAnimationState() == UpperBodyAnimationState.NONE)
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
                if (leftMouseClickTimer >= 1f && isGrap == false)
                {
                    // ��� �ִϸ��̼� �۵�
                    isGrap = true;
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.GRAP);
                }
            }
            // ���콺 ��Ŭ�� ��
            if (Input.GetMouseButtonUp(0))
            {
                if (isGrap == true)
                {
                    isGrap = false;
                    playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
                    PlayerController targetPlayerController = Managers.Player.FindPlayerById(playerStatus.GetGrapTargetPlayerId()).GetComponent<PlayerController>();
                    packetManager.SendPlayerThrowOtherPlayerPacket(playerStatus.GetId(), GetPosition(), GetDirection(),
                        playerStatus.GetGrapTargetPlayerId(), targetPlayerController.GetPosition(), targetPlayerController.GetDirection());
                }
                else if (playerStatus.GetUpperBodyAnimationState() == UpperBodyAnimationState.NONE)
                {
                    if (playerStatus.GetIsHaveWeapon() == false)
                    {
                        playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.ATTACK);
                    }
                    else
                    {
                        playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.WEAPONATTACK);
                    }
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
    public void s_PickUpBomb(int playerId, int bombId)
    {
        // ��ź�� �÷��̾� �ٿ��ֱ�
        Bomb targetBomb = Managers.BombObject.FindBombById(bombId).GetComponent<Bomb>();
        targetBomb.PickUp(playerId, bombInvenTransform);
        playerStatus.SetIsHaveBomb(true, bombId);
    }
    public void s_PickUpWeapon(int playerId, int weaponId)
    {
        // ����� �÷��̾� �ٿ��ֱ�
        Weapon targetWeapon = Managers.WeaponObject.FindWeaponById(weaponId).GetComponent<Weapon>();
        targetWeapon.PickUp(playerId);
        playerStatus.SetIsHaveWeapon(true, weaponId);
    }
    public void Jump()
    {
        isGrounded = false;
        pelvisRigidbody.velocity = Vector3.up * jumpForce;
    }

    public void Throw()
    {
        if(playerStatus.GetIsHaveBomb() == true)
        {
            GameObject targetBomb = Managers.BombObject.FindBombById(playerStatus.GetBombId());
            targetBomb.GetComponent<Bomb>().Throw(stabillizerDirection, playerStatus.GetStrength());

            // ������ �÷��̾� ��ġ, ��ź �߻� ����, ��ź ��ġ, �÷��̾� ���̵�, ��ź ���̵� ������
            packetManager.SendPlayerThrowBombPacket(targetBomb.transform.position, stabillizerDirection, myId, targetBomb.GetComponent<Bomb>().GetId());
            playerStatus.SetIsHaveBomb(false);

            playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        }
        else
        {
            Debug.Log("Player " + playerStatus.GetId() + " Don't Have an Item to Throw !!!");
        }
    }
    public void s_Throw(Vector3 bombPosition, int bombId)
    {
        if (playerStatus.GetIsHaveBomb() == true)
        {
            GameObject targetBomb = Managers.BombObject.FindBombById(bombId);
            targetBomb.transform.position = bombPosition;
            targetBomb.GetComponent<Bomb>().Throw(moveDirection, playerStatus.GetStrength());

            playerStatus.SetIsHaveBomb(false);
        }
        else
        {
            Debug.Log("Player " + playerStatus.GetId() + " Don't Have an Item to Throw !!!");
        }
    }
    
    public void SendToServerGoalTeamNumber(int bombId, int teamNumber)
    {
        packetManager.SendBombInputPacket(bombId, teamNumber);
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
    public Vector3 GetPosition()
    {
        return pelvis.transform.position;
    }
    public void SetDirection(Vector3 direction)
    {
        moveDirection = direction;
    }
    public Vector3 GetDirection()
    {
        return moveDirection;
    }
    public void SetIsMove(bool isMove)
    {
        this.isMove = isMove;
    }
    public void SetIsJump(bool isJump)
    {
        this.isJump = isJump;
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
    private void FallDownCheck()
    {
        if(pelvis.transform.position.y < -5f)
        {
            packetManager.SendPlayerDamageReceivePacket(playerStatus.GetId(), playerStatus.GetId(), -1, eDamageType.AT_FALLDOWN, Vector3.zero);
        }
    }

    public void Respawn(float x, float z)
    {
        playerStatus.SetIsDie(false);

        SetDirectionByTeam();

        Vector3 targetPos = new Vector3(x, createHeight, z);

        SetPosition(targetPos);

        Managers.Sound.Play("Sfx_Ch_Respawn");
    }
    public void ResetPlayerControllerSetting()
    {
        leftMouseClickTimer = 0f;
        isGrap = false;
        isGrounded = false;
        isLeftShiftKeyDown = false;
        beforeAxisRawH = 0;
        beforeAxisRawV = 0;
        SetIsMove(false);
        nowLowerBodyAnimationState = LowerBodyAnimationState.IDLE;
        isPickUpMode = false;
        targetItem = null;
    }

    private bool CheckHitWall()
    {
        float scope = 0.7f;

        // ���� Ȯ�ο�
        //Debug.DrawRay(pelvis.transform.position, moveDirection * scope, Color.red);

        List<Vector3> rayPositions = new List<Vector3>();

        rayPositions.Add(pelvis.transform.position);
        rayPositions.Add(head.transform.position);
        rayPositions.Add(clavicleL.transform.position);
        rayPositions.Add(clavicleR.transform.position);

        foreach (Vector3 pos in rayPositions)
        {
            if (Physics.Raycast(pos, moveDirection, out RaycastHit hit, scope) == true)
            {
                if (hit.collider.tag == "Ground")
                {
                    SetCanMove(false);
                    return true;
                }
            }
        }

        SetCanMove(true);
        return false;
    }
    private void SetCanMove(bool canImove)
    {
        if (canImove == false)
        {
            if (isGrounded == true)
            {
                pelvisRigidbody.constraints = RigidbodyConstraints.FreezePositionX |
                                              RigidbodyConstraints.FreezePositionZ;
            }
            pelvisRigidbody.velocity = Vector3.zero;
        }
        else
        {
            pelvisRigidbody.constraints = RigidbodyConstraints.None;
        }
    }

    public void GameStart()
    {
        gameStart = true;
    }

    public void GameEnd()
    {
        gameStart = false;
    }

    public void SetDirectionByTeam()
    {
        int teamNumber = playerStatus.GetTeamNumber();
        switch (teamNumber)
        {
            case 0:
                {
                    stabilizer.transform.eulerAngles = new Vector3(0.0f, 90f, 0.0f);
                }
                break;
            case 1:
                {
                    stabilizer.transform.eulerAngles = new Vector3(0.0f, 270f, 0.0f);
                }
                break;
            default:
                {
                    Debug.Log("ERORR!!! SetDirectionByTeam, Wrong Team Number !!!");
                }
                break;
        }
    }
    public void SetDirectionInResultScene()
    {
        stabilizer.transform.eulerAngles = new Vector3(0.0f, 180f, 0.0f);
    }

    public void s_GrapPlayer(int targetId, bool isLeftHand, Vector3 handPos, Vector3 targetHeadPos)
    {
        playerStatus.SetIsGrapPlayer(true, targetId);
        if(isLeftHand == true)
        {
            handL.GetComponent<AttackChecker>().GrapPlayer(targetId, handPos, targetHeadPos);
        }
        else
        {
            handR.GetComponent<AttackChecker>().GrapPlayer(targetId, handPos, targetHeadPos);
        }
    }
    public void s_ThrowPlayer()
    {
        playerStatus.SetIsGrapPlayer(false);
    }
    public void SetHeadPosition(Vector3 headPos)
    {
        head.transform.position = headPos;
    }
    public Vector3 GetHeadPosition()
    {
        return head.transform.position;
    }
    public Rigidbody GetHeadRigidbody()
    {
        return head.GetComponent<Rigidbody>();
    }
}
