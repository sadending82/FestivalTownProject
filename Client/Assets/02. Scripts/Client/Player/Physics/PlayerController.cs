using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ClientProtocol;
using NetworkProtocol;
using UnityEngine.Rendering;
using TMPro.Examples;
using static UnityEngine.GraphicsBuffer;
using UnityEngine.Playables;
using UnityEditor;
using ExcelDataStructure;
using Unity.VisualScripting;
using UnityEngine.UIElements;

public class PlayerController : MonoBehaviour
{
    [Header("--- Direct Connect ---")]
    public Transform cameraArm;
    public Transform stabilizer;
    public GameObject pelvis;
    private Rigidbody pelvisRigidbody;
    public GameObject clavicleL;
    public GameObject clavicleR;
    public GameObject head;
    public GameObject handL;
    public GameObject handR;
    public Rigidbody leftFootRigidbody;
    public Rigidbody rightFootRigidbody;

    [Header("--- State ---")]
    public float JumpForce;
    public float FloorDetectionDistance;
    public float RotateSpeed;

    private int myId;
    private bool amIPlayer;
    private CharacterStatus playerStatus;
    private float createHeight = 1;

    private bool isGrounded;
    private bool isJump;
    private bool isDash;
    private bool isFlyingKick;
    private bool recoveryStaminaChecker;

    private bool isMove;
    private Vector3 moveDirection;
    private Quaternion rotationQuaternion;
    private Vector3 stabillizerDirection;

    private float walkSpeed;
    private float runSpeed;
    private int runStaminaConsume;
    private float holdAndWalkSpeed;
    private int holdAndWalkStaminaConsume;
    private float holdAndRunSpeed;
    private int holdAndRunStaminaConsume;
    private float dashForce;
    private int dashStaminaConsume;
    private float dashCooltime = 1.0f;
    private bool dashCooltimeChecker;
    private int jumpStaminaConsume;
    private float jumpSpeedOffset = 0.5f;
    private bool doubleJumpChecker;
    private float flyingKickForce;
    private int flyingKickStaminaConsume;
    private bool flyingKickChecker;

    private bool isLeftShiftKeyDown;
    private bool beforeIsLeftShiftKeyDown;

    //------ Pick Up -------
    public Transform bombInvenTransform;
    [SerializeField] private NearObjectChecker nearObjectChecker;
    private float leftMouseClickTimer;
    private float fKeyDownTimer;
    private bool isPickUpMode;
    private bool isDropMode;
    private bool isGrap;
    private GameObject targetItem;
    private float bombPickUpSpeed;
    private float weaponPickUpSpeed;
    private float pickUpRange;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;
    private ReceiveManager receiveManager;
    private float beforeAxisRawH, beforeAxisRawV;
    private float AxisRawH, AxisRawV;
    private bool gameStart = false;

    //------ timer for send ------
    private float curTime= 0.0f;
    private float sendInterval = 0.01666666666f * 3;


    private void Awake()
    {
        ResetPlayerControllerSetting();
        amIPlayer = false;
        stabillizerDirection = Vector3.zero;

        network = Managers.Network;
        packetManager = network.GetPacketManager();
        receiveManager = network.GetReceiveManager();

        playerStatus = this.GetComponent<CharacterStatus>();
        myId = playerStatus.GetId();

        pelvisRigidbody = pelvis.GetComponent<Rigidbody>();

        nearObjectChecker = GetComponentInChildren<NearObjectChecker>();
    }
    void Start()
    {
        // 스탯 관련
        GameDataEntity data = Managers.Data.GetData(1000);
        CharacterStatEntity cse = (CharacterStatEntity)data;

        float chSpeed = (float)cse.Ch_Speed;
        float chStrength = (float)cse.Ch_Strength;

        // 이동 관련
        CharacterMoveEntity cme;

        data = Managers.Data.GetData(20002);
        cme = (CharacterMoveEntity)data;
        walkSpeed = cme.Value * chSpeed;

        data = Managers.Data.GetData(20003);
        cme = (CharacterMoveEntity)data;
        runSpeed = cme.Value * chSpeed;
        runStaminaConsume = cme.Ch_StaminaConsume;

        data = Managers.Data.GetData(20005);
        cme = (CharacterMoveEntity)data;
        dashForce = cme.Value * chSpeed;
        dashStaminaConsume = cme.Ch_StaminaConsume;

        data = Managers.Data.GetData(20006);
        cme = (CharacterMoveEntity)data;
        holdAndWalkSpeed = cme.Value * chSpeed;
        holdAndWalkStaminaConsume = cme.Ch_StaminaConsume;

        data = Managers.Data.GetData(20007);
        cme = (CharacterMoveEntity)data;
        holdAndRunSpeed = cme.Value * chSpeed;
        holdAndRunStaminaConsume = cme.Ch_StaminaConsume;

        data = Managers.Data.GetData(20004);
        cme = (CharacterMoveEntity)data;
        jumpStaminaConsume = cme.Ch_StaminaConsume;

        // 공격 관련
        CharacterAttackEntity cate;

        data = Managers.Data.GetData(10002);
        cate = (CharacterAttackEntity)data;
        flyingKickForce = cate.Value * chStrength;
        flyingKickStaminaConsume = cme.Ch_StaminaConsume;


        // 픽업 관련
        CharacterActionEntity cae;

        data = Managers.Data.GetData(30001);
        cae = (CharacterActionEntity)data;
        weaponPickUpSpeed = cae.Action_Speed;
        pickUpRange = cae.Action_Range;

        Vector3 basicScale = nearObjectChecker.transform.localScale;
        nearObjectChecker.transform.localScale = new Vector3(basicScale.x * pickUpRange, basicScale.y, basicScale.z * pickUpRange);

        data = Managers.Data.GetData(30002);
        cae = (CharacterActionEntity)data;
        bombPickUpSpeed = cae.Action_Speed;
    }
    public void ResetPlayerControllerSetting()
    {
        leftMouseClickTimer = 0f;
        fKeyDownTimer = 0f;
        isGrap = false;
        isJump = false;
        isDash = false;
        isFlyingKick = false;
        doubleJumpChecker = false;
        dashCooltimeChecker = false;
        flyingKickChecker = false;
        isPickUpMode = false;
        isDropMode = false;
        isGrounded = false;
        isLeftShiftKeyDown = false;
        beforeIsLeftShiftKeyDown = false;
        recoveryStaminaChecker = false;
        beforeAxisRawH = 0;
        beforeAxisRawV = 0;
        SetIsMove(false);
        targetItem = null;
    }

    private void FixedUpdate()
    {
        if (gameStart == true)
        {
            FallDownCheck();
            if (isGrounded == false && doubleJumpChecker == false)
            {
                CheckIsGround();
            }
            if (playerStatus.GetIsDie() == false)
            {
                // 살아있고 그로기 상태가 아님
                if (playerStatus.GetIsGroggy() == false)
                {
                    if (amIPlayer == true && pelvis.transform.position.y > 0)
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
                                if (playerStatus.GetIsGrapPlayer() == true)
                                {
                                    if (isGrounded == false)
                                    {
                                        pelvisRigidbody.velocity = moveDirection * holdAndWalkSpeed * jumpSpeedOffset;
                                    }
                                    else
                                    {
                                        pelvisRigidbody.velocity = moveDirection * holdAndWalkSpeed;
                                    }
                                }
                                else
                                {
                                    if (isGrounded == false)
                                    {
                                        pelvisRigidbody.velocity = moveDirection * walkSpeed * jumpSpeedOffset;
                                    }
                                    else
                                    {
                                        pelvisRigidbody.velocity = moveDirection * walkSpeed;
                                    }
                                }
                            }
                            else if (playerStatus.GetLowerBodyAnimationState() == LowerBodyAnimationState.RUN)
                            {
                                if (playerStatus.GetIsGrapPlayer() == true)
                                {
                                    if (isGrounded == false)
                                    {
                                        pelvisRigidbody.velocity = moveDirection * holdAndRunSpeed * jumpSpeedOffset;
                                    }
                                    else
                                    {
                                        pelvisRigidbody.velocity = moveDirection * holdAndRunSpeed;
                                    }
                                }
                                else
                                {
                                    if (isGrounded == false)
                                    {
                                        pelvisRigidbody.velocity = moveDirection * runSpeed * jumpSpeedOffset;
                                    }
                                    else
                                    {
                                        pelvisRigidbody.velocity = moveDirection * runSpeed;
                                    }
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
                            if (isDash == true)
                            {
                                if (isGrounded == true)
                                {
                                    Dash(moveDirection);
                                    isDash = false;
                                }
                            }
                            if (isFlyingKick == true)
                            {
                                if (isGrounded == false)
                                {
                                    FlyingKick(moveDirection);
                                    isFlyingKick = false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    private void Update()
    {
        if (gameStart == true)
        {
            // 테스트
            if (amIPlayer == true)
            {
                if (Input.GetKeyUp(KeyCode.T))
                {
                    packetManager.SendPlayerCollisionToBlockPacket(playerStatus.GetId());
                }
            }

            if (playerStatus.GetIsDie() == false)
            {
                if (amIPlayer == true)
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
            }
            else if (amIPlayer == true)
            {
                SpectatorCameraControl();
            }
        }
    }

    private void SendForSync()
    {
        if (pelvis != null && gameStart == true)
        {
            packetManager.SendPlayerSyncPacket(pelvis.transform.position, stabillizerDirection, playerStatus.GetStamina(), myId);
        }
    }
    private void Move()
    {
        isLeftShiftKeyDown = Input.GetKey(KeyCode.LeftShift);

        AxisRawH = Input.GetAxisRaw("Horizontal");
        AxisRawV = Input.GetAxisRaw("Vertical");
        Vector3 moveInput = new Vector3(AxisRawH, AxisRawV, 0);
        Vector3 lookForward = new Vector3(cameraArm.forward.x, 0f, cameraArm.forward.z).normalized;
        Vector3 lookRight = new Vector3(cameraArm.right.x, 0f, cameraArm.right.z).normalized;
        moveDirection = lookForward * moveInput.y + lookRight * moveInput.x;
        moveDirection = moveDirection.normalized;

        if (stabillizerDirection != moveDirection && moveDirection != Vector3.zero)
        {
            stabillizerDirection = moveDirection;
            rotationQuaternion = Quaternion.LookRotation(moveDirection);
            stabilizer.rotation = rotationQuaternion;
        }

        if (AxisRawH != beforeAxisRawH || AxisRawV != beforeAxisRawV || beforeIsLeftShiftKeyDown != isLeftShiftKeyDown || recoveryStaminaChecker == true)
        {
            recoveryStaminaChecker = false;
            if (moveInput == Vector3.zero)
            {
                if (pelvis != null)
                {
                    packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_MOVESTOP);
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
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
                    if (playerStatus.GetIsGrapPlayer() == true)
                    {
                        if (isLeftShiftKeyDown == true && holdAndRunStaminaConsume <= playerStatus.GetStamina())
                        {
                            playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                            packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_RUN);
                        }
                        else if (holdAndWalkStaminaConsume <= playerStatus.GetStamina())
                        {
                            playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                            packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_WALK);
                        }
                        else
                        {
                            playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
                            packetManager.SendPlayerStopPacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_MOVESTOP);
                        }
                    }
                    else
                    {
                        if (isLeftShiftKeyDown == true && runStaminaConsume <= playerStatus.GetStamina())
                        {
                            playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                            packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_RUN);
                        }
                        else
                        {
                            playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                            packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_WALK);
                        }
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
        beforeIsLeftShiftKeyDown = isLeftShiftKeyDown;

        if (moveInput != Vector3.zero)
        {
            if (isLeftShiftKeyDown)
            {
                if (CheckHitWall() == false)
                {
                    if (playerStatus.GetIsGrapPlayer() == true && holdAndRunStaminaConsume <= playerStatus.GetStamina())
                    {
                        if (isGrounded == false)
                        {
                            pelvisRigidbody.velocity = moveDirection * holdAndRunSpeed * jumpSpeedOffset;
                        }
                        else
                        {
                            pelvisRigidbody.velocity = moveDirection * holdAndRunSpeed;
                        }
                    }
                    else if (runStaminaConsume <= playerStatus.GetStamina())
                    {
                        if (isGrounded == false)
                        {
                            pelvisRigidbody.velocity = moveDirection * runSpeed * jumpSpeedOffset;
                        }
                        else
                        {
                            pelvisRigidbody.velocity = moveDirection * runSpeed;
                        }
                    }
                    else
                    {
                        if (isGrounded == false)
                        {
                            pelvisRigidbody.velocity = moveDirection * walkSpeed * jumpSpeedOffset;
                        }
                        else
                        {
                            pelvisRigidbody.velocity = moveDirection * walkSpeed;
                        }
                    }
                }
            }
            else
            {
                if (CheckHitWall() == false)
                {
                    if (playerStatus.GetIsGrapPlayer() == true && holdAndWalkStaminaConsume <= playerStatus.GetStamina())
                    {
                        if (isGrounded == false)
                        {
                            pelvisRigidbody.velocity = moveDirection * holdAndWalkSpeed * jumpSpeedOffset;
                        }
                        else
                        {
                            pelvisRigidbody.velocity = moveDirection * holdAndWalkSpeed;
                        }
                    }
                    else
                    {
                        if (isGrounded == false)
                        {
                            pelvisRigidbody.velocity = moveDirection * walkSpeed * jumpSpeedOffset;
                        }
                        else
                        {
                            pelvisRigidbody.velocity = moveDirection * walkSpeed;
                        }
                    }
                }
            }
        }
        // Jump
        if (Input.GetAxis("Jump") > 0 && playerStatus.GetStamina() >= jumpStaminaConsume)
        {
            if (isGrounded == true)
            {
                if (pelvis != null)
                {
                    playerStatus.ConsumeStamina(jumpStaminaConsume);
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
    public void Jump()
    {
        isGrounded = false;
        doubleJumpChecker = true;
        pelvisRigidbody.AddForce(Vector3.up * JumpForce, ForceMode.Impulse);
        flyingKickChecker = false;
        StartCoroutine(WaitIsGroundCheck());
    }
    /// <summary>
    /// 더블 점프 방지를 위한 함수
    /// </summary>
    IEnumerator WaitIsGroundCheck()
    {
        yield return new WaitForSeconds(0.5f);
        doubleJumpChecker = false;
    }
    public void Dash(Vector3 direction)
    {
        dashCooltimeChecker = true;
        pelvisRigidbody.AddForce(direction * dashForce * 10, ForceMode.Impulse);
        StartCoroutine(WaitDashCooltime());
    }
    /// <summary>
    /// 대쉬 쿨타임 적용
    /// </summary>
    IEnumerator WaitDashCooltime()
    {
        yield return new WaitForSeconds(dashCooltime);
        dashCooltimeChecker = false;
    }
    public void FlyingKick(Vector3 direction)
    {
        flyingKickChecker = true;
        head.GetComponent<Rigidbody>().AddForce(-direction * flyingKickForce * 5, ForceMode.Impulse);
        pelvisRigidbody.AddForce(direction * flyingKickForce * 10, ForceMode.Impulse);
    }
    public void SetWalkState()
    {
        packetManager.SendPlayerMovePacket(pelvis.transform.position, stabillizerDirection, myId, ePlayerMoveState.PS_WALK);
        playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
    }
    public void RecoveryStaminaCheckerOn()
    {
        recoveryStaminaChecker = true;
    }

    private void KeyboardInput()
    {
        // F 키를 눌렀을때 플레이어가 아이템을 가지지 않고 
        // 주울 수 있는 범위 내에 아이템이 존재하면 픽업모드 시작
        if (Input.GetKeyDown(KeyCode.F))
        {
            if (nearObjectChecker.GetNearObject() != null &&
                (playerStatus.GetIsHaveBomb() == false && playerStatus.GetIsHaveWeapon() == false) &&
                (nearObjectChecker.GetNearObject().tag == "Weapon" || nearObjectChecker.GetNearObject().tag == "Bomb"))
            {
                fKeyDownTimer = 0;
                isPickUpMode = true;
                targetItem = nearObjectChecker.GetNearObject();
            }
            else if ((playerStatus.GetIsHaveBomb() == true || playerStatus.GetIsHaveWeapon() == true) && isPickUpMode == false)
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
                // 목표 아이템이 사라졌거나 변경 됐으면 픽업모드 초기화
                if (nearObjectChecker.GetNearObject() == null || targetItem != nearObjectChecker.GetNearObject())
                {
                    fKeyDownTimer = 0;
                    isPickUpMode = false;
                }
                else
                {
                    if (targetItem.tag == "Bomb" && playerStatus.GetIsHaveBomb() == false && fKeyDownTimer >= bombPickUpSpeed)
                    {
                        Bomb targetBomb = targetItem.GetComponent<Bomb>();
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerGrabBombPacket(pelvis.transform.position, stabillizerDirection, myId, targetBomb.GetId());
                        isPickUpMode = false;
                    }
                    else if (targetItem.tag == "Weapon" && playerStatus.GetIsHaveWeapon() == false && fKeyDownTimer >= weaponPickUpSpeed)
                    {
                        Weapon targetWeapon = targetItem.GetComponent<Weapon>();
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerGrabWeaponPacket(pelvis.transform.position, stabillizerDirection, myId, targetWeapon.GetId());
                        isPickUpMode = false;
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
                        isDropMode = false;
                    }
                    if (playerStatus.GetIsHaveBomb() == true)
                    {
                        fKeyDownTimer = 0;
                        packetManager.SendPlayerDropBombPacket(GetPosition(), myId, playerStatus.GetBombId());
                        isDropMode = false;
                    }
                }
            }
        }
    }
    private void MouseInput()
    {
        // 마우스 좌클릭 다운
        if (Input.GetMouseButtonDown(0))
        {
            leftMouseClickTimer = 0f;
            
            // 날라차기
            if(isLeftShiftKeyDown == true && isGrounded == false)
            {
                if (playerStatus.GetStamina() >= flyingKickStaminaConsume && moveDirection != Vector3.zero)
                {
                    if (pelvis != null)
                    {
                        playerStatus.ConsumeStamina(flyingKickStaminaConsume);
                        packetManager.SendPlayerMovePacket(pelvis.transform.position, moveDirection, myId, ePlayerMoveState.PS_FLYING_KICK);
                    }
                    else
                    {
                        Debug.Log("Not Send Dash Packet, Pelvis is Null !!!");
                    }
                    FlyingKick(moveDirection);
                }
            }
        }
        // 마우스 좌클릭 홀드
        if (Input.GetMouseButton(0))
        {
            leftMouseClickTimer += Time.deltaTime;
            if (leftMouseClickTimer >= 1f && isGrap == false)
            {
                // 잡기 애니메이션 작동
                isGrap = true;
                playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.GRAP);
            }
        }
        // 마우스 좌클릭 업
        if (Input.GetMouseButtonUp(0))
        {
            if (isGrap == true)
            {
                isGrap = false;
                playerStatus.SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);

                if (playerStatus.GetIsGrapPlayer() == true)
                {
                    GrapOff();
                }
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

        // 마우스 우클릭 다운
        if (Input.GetMouseButtonDown(1))
        {
            // 수정 : 여기 애니메이션 Throw 있어야함
            GameObject targetBomb = Managers.BombObject.FindBombById(playerStatus.GetBombId());
            if (targetBomb != null)
            {
                packetManager.SendPlayerThrowBombPacket(targetBomb.transform.position, stabillizerDirection, myId, targetBomb.GetComponent<Bomb>().GetId());
            }
        }

        // 마우스 휠클릭 다운(대쉬)
        if (Input.GetMouseButtonDown(2))
        {
            if (isGrounded == true && isLeftShiftKeyDown == true && dashCooltimeChecker == false
                && playerStatus.GetStamina() >= dashStaminaConsume && moveDirection != Vector3.zero)
            {
                if (pelvis != null)
                {
                    playerStatus.ConsumeStamina(dashStaminaConsume);
                    packetManager.SendPlayerMovePacket(pelvis.transform.position, moveDirection, myId, ePlayerMoveState.PS_DASH);
                }
                else
                {
                    Debug.Log("Not Send Dash Packet, Pelvis is Null !!!");
                }
                Dash(moveDirection);
            }
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
    public bool GetisGrounded()
    {
        return isGrounded;
    }

    private void CheckIsGround()
    {
        Ray rayL = new Ray(leftFootRigidbody.position, Vector3.down);
        Ray rayR = new Ray(rightFootRigidbody.position, Vector3.down);

        RaycastHit hitInfoL, hitinfoR;
        if (Physics.Raycast(rayL, out hitInfoL, FloorDetectionDistance) == true)
        {
            if (hitInfoL.collider.gameObject.tag == "Ground")
            {
                if (pelvis != null && amIPlayer == true)
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
                isGrounded = true;
            }
        }
        else if (Physics.Raycast(rayR, out hitinfoR, FloorDetectionDistance) == true)
        {
            if (hitinfoR.collider.gameObject.tag == "Ground")
            {
                if (pelvis != null && amIPlayer == true)
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
                isGrounded = true;
            }
        }
    }
    private bool CheckHitWall()
    {
        float scope = 0.7f;

        // 레이 확인용
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
    private void FallDownCheck()
    {
        if (pelvis.transform.position.y < -10f)
        {
            if (playerStatus.GetIsDie() == false)
            {
                packetManager.SendPlayerDamageReceivePacket(playerStatus.GetId(), playerStatus.GetId(), -1, eDamageType.AT_FALLDOWN, Vector3.zero);
            }
        }
    }
    private void SpectatorCameraControl()
    {
        if (Input.GetMouseButtonUp(0) || Input.GetKeyDown(KeyCode.Tab))
        {
            Managers.SpectatorCamera.SwitchNextCamera();
        }
    }
    public void s_PickUpBomb(int playerId, int bombId)
    {
        // 폭탄과 플레이어 붙여주기
        Bomb targetBomb = Managers.BombObject.FindBombById(bombId).GetComponent<Bomb>();
        targetBomb.PickUp(playerId, bombInvenTransform);
        playerStatus.SetIsHaveBomb(true, bombId);
    }
    public void s_PickUpWeapon(int playerId, int weaponId)
    {
        // 무기와 플레이어 붙여주기
        Weapon targetWeapon = Managers.WeaponObject.FindWeaponById(weaponId).GetComponent<Weapon>();

        targetWeapon.PickUp(playerId);
        playerStatus.SetIsHaveWeapon(true, weaponId);
    }
    public void s_Throw(Vector3 bombPosition, int bombId)
    {
        if (playerStatus.GetIsHaveBomb() == true)
        {
            GameObject targetBomb = Managers.BombObject.FindBombById(bombId);
            targetBomb.transform.position = bombPosition;
            targetBomb.GetComponent<Bomb>().Throw(moveDirection, playerStatus.GetThrowPower());

            playerStatus.SetIsHaveBomb(false);
        }
    }
    public void s_GrapPlayer(int targetId, bool isLeftHand, Vector3 handPos, Vector3 targetHeadPos)
    {
        playerStatus.SetIsGrapPlayer(true, targetId);

        CharacterStatus targetPlayerState = Managers.Player.FindPlayerById(targetId).GetComponent<CharacterStatus>();
        targetPlayerState.SetIsGrapped(true, playerStatus.GetId());

        if (isLeftHand == true)
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
        handL.GetComponent<AttackChecker>().ThrowPlayer();
        handR.GetComponent<AttackChecker>().ThrowPlayer();
    }
    public void GrapOff()
    {
        PlayerController targetPlayerController = Managers.Player.FindPlayerById(playerStatus.GetGrapTargetPlayerId()).GetComponent<PlayerController>();
        packetManager.SendPlayerThrowOtherPlayerPacket(playerStatus.GetId(), GetPosition(), GetDirection(),
            playerStatus.GetGrapTargetPlayerId(), targetPlayerController.GetPosition(), targetPlayerController.GetDirection());
    }
    public void FirstSpawn(float x, float z)
    {
        SetDirectionByTeam();

        Vector3 targetPos = new Vector3(x, createHeight, z);

        SetPosition(targetPos);

        Managers.Sound.Play("Sfx_Ch_Respawn");
    }
    public void Respawn(float x, float z)
    {
        playerStatus.SetIsDie(false);

        SetDirectionByTeam();

        Vector3 targetPos = new Vector3(x, createHeight, z);

        SetPosition(targetPos);

        targetPos = new Vector3(x, 0.01f, z);
        Managers.Effect.PlayEffect("Ch_Respawn", targetPos);
        Managers.Sound.Play("Sfx_Ch_Respawn");
    }
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
    }
    public void SetPosition(Vector3 position)
    {
        if (pelvis == null) 
        {
            Debug.Log("pelvis Null!!!!");
            return; 
        }

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
    public void SetIsDash(bool isDash)
    {
        this.isDash = isDash;
    }
    public void SetIsFlyingKick(bool isFlyingKick)
    {
        this.isFlyingKick = isFlyingKick;
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
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.WALK);
                }
                break;
            case ePlayerMoveState.PS_RUN:
                {
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.RUN);
                }
                break;
            case ePlayerMoveState.PS_MOVESTOP:
                {
                    playerStatus.SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
                }
                break;
            default:
                // Jump Stop, Jump
                break;
        }
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