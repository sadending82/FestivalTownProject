using ActiveRagdoll;
using ClientProtocol;
using ExcelDataStructure;
using UnityEngine;
using UnityEngine.TextCore.Text;
using static UnityEngine.GraphicsBuffer;

public class CharacterStatus : MonoBehaviour
{
    private Camera myCamera;
    private PlayerController playerController;
    private bool isGameEnd;

    // 스탯
    [Header("--- Status ---")]
    [SerializeField] private int id;
    [SerializeField] private int teamNumber;
    [Space(10f)]
    [SerializeField] private float maxHp;
    [SerializeField] private float hp;
    [Space(10f)]
    [SerializeField] private int maxStamina;
    [SerializeField] private int stamina;
    [Space(10f)]
    public int AttackedMotionPower;
    public float throwPower;

    private bool isDie = true;
    private bool amIPlayer;

    // 픽업 관련
    [Header("--- PickUp ---")]
    [SerializeField] private bool isHaveBomb = false;
    private int bombId;
    [SerializeField] private bool isHaveWeapon = false;
    private int weaponId;
    private GameObject myWeapon;
    [SerializeField] private bool isGrapPlayer = false;
    private int grapTargetPlayerId;
    private bool isGrapped = false;
    private int grappedPlayerId;

    // 애니메이션 컨트롤
    private bool isGroggy;
    private AnimationController animationController;
    private ActiveRagdoll.AnimationModule animationMoudule;
    private UpperBodyAnimationState upperBodyAnimationState; // 상체의 상태는 서버와 교환
    private LowerBodyAnimationState lowerBodyAnimationState; // 하체의 상태는 클라가 관리(이동 관련해서는 이미 서버와 교환하고 있기 때문)

    // 바로 연결
    [Header("--- Direct Connect ---")]
    public GameObject pelvis;
    public Rigidbody headRig;
    public GameObject hitbox;
    public GameObject playerMesh;
    public GameObject weaponInven;

    // 서버
    private NetworkManager network;
    private PacketManager packetManager;


    private void Awake()
    {
        myCamera = GetComponentInChildren<Camera>();
        animationController = GetComponentInChildren<AnimationController>();
        animationMoudule = GetComponent<ActiveRagdoll.AnimationModule>();
        playerController = GetComponent<PlayerController>();

        amIPlayer = false;
        myCamera.enabled = false;
        isGameEnd = false;

        ResetCharacterState();
    }

    private void Start()
    {
        network = Managers.Network;
        packetManager = network.GetPacketManager();
    }

    public void GameStart()
    {
        isGameEnd = false;
        playerController.GameStart();
        animationController.GameStart();
        myCamera.gameObject.GetComponent<PlayerCameraController>().GameStart();
    }
    public void GameEnd()
    {
        isGameEnd = true;
        playerController.GameEnd();
        animationController.GameEnd();
        myCamera.gameObject.GetComponent<PlayerCameraController>().GameEnd();
    }

    // ------------------- Status -------------------
    public void ResetCharacterState()
    {
        var data = Managers.Data.GetData(1000);
        CharacterStatEntity cse = (CharacterStatEntity)data;
        
        SetMaxHp(cse.Ch_Hp);
        SetHp(cse.Ch_Hp);
        SetMaxStamina(cse.Ch_Stamina);
        SetStamina(cse.Ch_Stamina);
        SetIsHaveBomb(false);
        SetIsHaveWeapon(false);
        SetIsGrapPlayer(false);
        SetIsGrapped(false);
        GroggyOff();
        SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
        if (amIPlayer == true)
        {
            CancelInvoke("StaminaUpdate");
            InvokeRepeating("StaminaUpdate", 0, 1);
        }

        playerController.ResetPlayerControllerSetting();
    }
    public void SetId(int id)
    {
        this.id = id;
        myCamera.GetComponent<PlayerCameraController>().SetMyId(id);
    }
    public int GetId()
    {
        return id;
    }
    public void SetTeamNumber(int teamNumber)
    {
        this.teamNumber = teamNumber;
    }
    public int GetTeamNumber()
    {
        return teamNumber;
    }
    public void SetMaxHp(float maxHp)
    {
        this.maxHp = maxHp;
    }
    public float GetMaxHp()
    {
        return maxHp;
    }
    public void SetHp(float hp)
    {
        this.hp = hp;
    }
    public float GetHp()
    {
        return hp;
    }
    public void SetMaxStamina(int maxStamina)
    {
        this.maxStamina = maxStamina;
    }
    public int GetMaxStamina()
    {
        return maxStamina;
    }
    public void SetStamina(int stamina)
    {
        this.stamina = stamina;
    }
    public int GetStamina()
    {
        return stamina;
    }
    /// <summary>
    /// 스테미나 증감량 계산해주는 함수
    /// </summary>
    private int CalculateStaminaIncrease()
    {
        GameDataEntity data;
        CharacterMoveEntity cme;
        int staminaConsumption = 0;
        int staminaConsumptionByHold = 0;
        int staminaRecoveryAmount = 0;

        // 스테미나 관련 계산
        switch (lowerBodyAnimationState)
        {
            case LowerBodyAnimationState.IDLE:
                {
                    if (playerController.GetisGrounded() == true)
                    {
                        data = Managers.Data.GetData(20001);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    else
                    {
                        data = Managers.Data.GetData(20004);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    break;
                }
            case LowerBodyAnimationState.WALK:
                {
                    if (playerController.GetisGrounded() == true)
                    {
                        data = Managers.Data.GetData(20002);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    else
                    {
                        data = Managers.Data.GetData(20004);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    break;
                }
            case LowerBodyAnimationState.RUN:
                {
                    if (playerController.GetisGrounded() == true)
                    {
                        data = Managers.Data.GetData(20003);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    else
                    {
                        data = Managers.Data.GetData(20004);
                        cme = (CharacterMoveEntity)data;
                        staminaConsumption = cme.Ch_StaminaConsume;
                        staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    }
                    break;
                }
        }
        if (isGrapPlayer == true)
        {
            data = Managers.Data.GetData(10004);
            CharacterAttackEntity cae = (CharacterAttackEntity)data;
            staminaConsumptionByHold = cae.Ch_StaminaConsume;
        }
        else
        {
            staminaConsumptionByHold = 0;
        }
        return (staminaRecoveryAmount - staminaConsumption - staminaConsumptionByHold);
    }
    private void StaminaUpdate()
    {
        int staminaIncrease = CalculateStaminaIncrease();

        if(stamina == 0 && (stamina + staminaIncrease) > 0)
        {
            playerController.RecoveryStaminaCheckerOn();
        }
        stamina += staminaIncrease;
        if (stamina <= 0)
        {
            stamina = 0;
            if (isGrapPlayer == true)
            {
                playerController.GrapOff();
            }
            if(lowerBodyAnimationState == LowerBodyAnimationState.RUN)
            {
                playerController.SetWalkState();
            }
        }
        else if (stamina > maxStamina)
        {
            stamina = maxStamina;
        }
    }
    public float GetThrowPower()
    {
        return throwPower;
    }
    public void SetIsDie(bool isDie)
    {
        this.isDie = isDie;
        if (isDie == true)
        {
            hp = 0;
            playerMesh.SetActive(false);
            Managers.Sound.Play("Sfx_Ch_Die");

            // 관전 관련
            if (amIPlayer == true)
            {
                Managers.SpectatorCamera.SwitchNextCamera();
            }
            else
            {
                Managers.SpectatorCamera.CheckSpectator(id);
            }
        }
        else
        {
            ResetCharacterState();
            playerMesh.SetActive(true);

            // 관전 관련
            if (amIPlayer == true)
            {
                Managers.SpectatorCamera.SwitchMyCamera();
            }
        }
    }
    public bool GetIsDie()
    {
        return isDie;
    }
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
        this.GetComponent<PlayerController>().SetAmIPlayer(amIPlayer);
        myCamera.GetComponent<PlayerCameraController>().SetAmIPlayer(amIPlayer);
        if (amIPlayer == true)
        {
            myCamera.enabled = true;
            // 로딩 창 없애기
            Managers.UI.ClosePopUpUI(GameObject.FindObjectOfType<UI_Loading>());

            headRig.gameObject.GetComponent<AudioListener>().enabled = true;
        }
    }
    public bool GetAmIPlayer()
    {
        return amIPlayer;
    }

    // ------------------- PickUp -------------------
    public void SetIsHaveBomb(bool isHaveBomb, int bombId = -1)
    {
        this.isHaveBomb = isHaveBomb;
        this.bombId = bombId;
    }
    public bool GetIsHaveBomb()
    {
        return isHaveBomb;
    }
    public int GetBombId()
    {
        return bombId;
    }
    public void SetIsHaveWeapon(bool isHaveWeapon, int weaponId = -1)
    {
        if (isHaveWeapon == true)
        {
            myWeapon = Managers.WeaponObject.FindWeaponById(weaponId);
        }
        else
        {
            myWeapon = null;
        }
        this.isHaveWeapon = isHaveWeapon;
        this.weaponId = weaponId;
    }
    public bool GetIsHaveWeapon()
    {
        return isHaveWeapon;
    }
    public int GetWeaponId()
    {
        return weaponId;
    }
    public GameObject GetWeaponInven()
    {
        return weaponInven;
    }

    public GameObject GetMyWeapon()
    {
        return myWeapon;
    }
    // 내가 잡은 플레이어
    public void SetIsGrapPlayer(bool isGrapPlayer, int targetPlayerId = -1)
    {
        this.isGrapPlayer = isGrapPlayer;
        this.grapTargetPlayerId = targetPlayerId;
    }
    public int GetGrapTargetPlayerId()
    {
        return grapTargetPlayerId;
    }
    public bool GetIsGrapPlayer()
    {
        return isGrapPlayer;
    }
    // 나를 잡은 플레이어
    public void SetIsGrapped(bool isGrapped, int grappedPlayerId = -1)
    {
        this.isGrapped = isGrapped;
        this.grappedPlayerId = grappedPlayerId;
    }
    public bool GetIsGrapped()
    {
        return isGrapped;
    }
    public int GetGrappedPlayerId()
    {
        return grappedPlayerId;
    }

    // ------------------- Animation Control -------------------
    public void GroggyOn()
    {
        if (isGroggy == false)
        {
            SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
            SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
            isGroggy = true;
            animationMoudule.GroggyOn();
        }
    }
    public void GroggyOff()
    {
        if (isGroggy == true)
        {
            if (isGrapped == true)
            {
                PlayerController targetPlayerController = Managers.Player.FindPlayerById(grappedPlayerId).GetComponent<PlayerController>();
                targetPlayerController.GrapOff();
            }

            isGroggy = false;
            SetStamina(maxStamina);
            animationMoudule.GroggyOff();
        }
    }
    public bool GetIsGroggy()
    {
        return isGroggy;
    }
    public void SetLayer(int id)
    {
        string layerName = "Player" + id;
        int layer = LayerMask.NameToLayer(layerName);
        hitbox.layer = layer;

        ChangeLayerRecursively(pelvis, layer);
    }
    /// <summary>
    /// 모든 자식 오브젝트에 접근해 레이어를 바꾸는 함수
    /// </summary>
    private void ChangeLayerRecursively(GameObject obj, int layer)
    {
        obj.layer = layer;

        foreach (Transform child in obj.transform)
        {
            ChangeLayerRecursively(child.gameObject, layer);
        }
    }
    public void SetUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        if (this.upperBodyAnimationState != upperBodyAnimationState && isGroggy == false)
        {
            this.upperBodyAnimationState = upperBodyAnimationState;
            packetManager.SendPlayerAnimationPacket(playerController.GetPosition(), playerController.GetDirection(), id, upperBodyAnimationState);
            animationController.SetUpperBodyAnimationState(upperBodyAnimationState);
        }
    }
    public void s_SetUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        if (this.upperBodyAnimationState != upperBodyAnimationState && isGroggy == false)
        {
            this.upperBodyAnimationState = upperBodyAnimationState;
            animationController.SetUpperBodyAnimationState(upperBodyAnimationState);
        }
    }
    public UpperBodyAnimationState GetUpperBodyAnimationState()
    {
        return this.upperBodyAnimationState;
    }

    public void SetLowerBodyAnimationState(LowerBodyAnimationState lowerBodyAnimationState)
    {
        if (this.lowerBodyAnimationState != lowerBodyAnimationState && isGroggy == false)
        {
            this.lowerBodyAnimationState = lowerBodyAnimationState;
            animationController.SetLowerBodyAnimationState(lowerBodyAnimationState);
        }
    }
    public LowerBodyAnimationState GetLowerBodyAnimationState()
    {
        return this.lowerBodyAnimationState;
    }
    public void Attacked(Vector3 direction)
    {
        headRig.AddForce(direction * AttackedMotionPower, ForceMode.Impulse);
    }
    public Vector3 GetAttackedDirection(Vector3 attackerPosition)
    {
        Vector3 attackedDirection = attackerPosition - pelvis.transform.position;
        attackedDirection.y = 0;

        return attackedDirection.normalized;
    }

    // ------------------- Camera -------------------
    public Camera GetMyCamera()
    {
        return myCamera;
    }
    public void CameraOn()
    {
        myCamera.enabled = true;
    }
    public void CameraOff()
    {
        myCamera.enabled = false;
    }

    // ------------------- Physic --------------------
    public Rigidbody GetHeadRigidbody()
    {
        return headRig;
    }
    public GameObject GetPelvis()
    {
        return pelvis;
    }

    // ------------------- Style --------------------
    public void SetStyle(Material skinMaterial, Material faceMaterial)
    {
        Material[] mat = new Material[] { skinMaterial, faceMaterial };
        playerMesh.GetComponent<SkinnedMeshRenderer>().materials = mat;
    }
}