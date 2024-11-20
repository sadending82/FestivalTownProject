using ActiveRagdoll;
using ClientProtocol;
using ExcelDataStructure;
using NetworkProtocol;
using System.IO;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.TextCore.Text;
using static UnityEngine.GraphicsBuffer;

public class CharacterStatus : MonoBehaviour
{
    private Camera myCamera;
    private PlayerController playerController;
    private PlayerUIController playerUIController;
    private bool isGameEnd;

    private const float RESPAWN_DELAY_TIME = 1.0f;

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
    public ParticleSystem RunEffect;

    // 커스터마이징
    [Header("--- Customizing ---")]
    public GameObject Head_Equipments;
    public GameObject Face_Equipments;
    public GameObject Back_Equipments;
    private Material skinMaterial;
    private Material faceMaterial;
    private GameObject headItem;
    private GameObject faceItem;
    private GameObject backItem;

    // 서버
    private NetworkManager network;
    private PacketManager packetManager;


    private void Awake()
    {
        myCamera = GetComponentInChildren<Camera>();
        playerUIController = GetComponentInChildren<PlayerUIController>();
        animationController = GetComponentInChildren<AnimationController>();
        animationMoudule = GetComponent<ActiveRagdoll.AnimationModule>();
        playerController = GetComponent<PlayerController>();

        amIPlayer = false;
        myCamera.enabled = false;
        isGameEnd = false;

        if(skinMaterial == null || faceMaterial == null)
        {
            Material[] mat = playerMesh.GetComponent<SkinnedMeshRenderer>().materials;
            skinMaterial = mat[0];
            faceMaterial = mat[1];
        }
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
        isDie = false;
        RunEffect.gameObject.SetActive(true);
        RunEffect.Stop();
        SetUITeam();
        SetUIMe();
        ResetCharacterState();
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
            Managers.SpectatorCamera.SetCameraInPlayerUIControllers();
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
                    data = Managers.Data.GetData(20001);
                    cme = (CharacterMoveEntity)data;
                    staminaConsumption = cme.Ch_StaminaConsume;
                    staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    break;
                }
            case LowerBodyAnimationState.WALK:
                {
                    data = Managers.Data.GetData(20002);
                    cme = (CharacterMoveEntity)data;
                    staminaConsumption = cme.Ch_StaminaConsume;
                    staminaRecoveryAmount = cme.Ch_Stamina_recovery;
                    break;
                }
            case LowerBodyAnimationState.RUN:
                {
                    data = Managers.Data.GetData(20003);
                    cme = (CharacterMoveEntity)data;
                    staminaConsumption = cme.Ch_StaminaConsume;
                    staminaRecoveryAmount = cme.Ch_Stamina_recovery;
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
    public void ConsumeStamina(int staminaConsume)
    {
        stamina -= staminaConsume;
        if(stamina < 0)
        {
            stamina = 0;
        }
    }
    public float GetThrowPower()
    {
        return throwPower;
    }
    public Vector3 GetDirection()
    {
        return playerController.GetDirection();
    }
    public void SetIsDie(bool isDie)
    {
        if (isDie == true)
        {
            hp = 0;
            playerMesh.SetActive(false);
            HideItem();
            playerUIController.gameObject.SetActive(false);
            Managers.Sound.Play("Sfx_Ch_Die");
            // 관전 관련
            if (amIPlayer == true)
            {
                Managers.SpectatorCamera.SwitchNextCamera();
                Managers.PostProcess.SetScreenGray();
            }
            else
            {
                Managers.SpectatorCamera.CheckSpectator(id);
            }
            this.isDie = isDie;
        }
        else
        {
            ResetCharacterState();
            playerMesh.SetActive(true);
            ShowItem();
            playerUIController.gameObject.SetActive(true);
            Managers.SpectatorCamera.SetCameraInPlayerUIControllers();

            // 관전 관련
            if (amIPlayer == true)
            {
                Managers.SpectatorCamera.SwitchMyCamera();
                Managers.PostProcess.SetScreenNormal();
            }

            Invoke("RespawnDelay", RESPAWN_DELAY_TIME);
        }
    }
    private void RespawnDelay()
    {
        isDie = false;
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
            Managers.Effect.PlayEffect("Ch_Groggy", playerUIController.gameObject.transform.position);
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
            if(lowerBodyAnimationState == LowerBodyAnimationState.RUN)
            {
                RunEffect.Play();
            }
            else
            {
                RunEffect.Stop();
            }
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
        pelvis.GetComponent<Rigidbody>().AddForce(direction * AttackedMotionPower * 0.5f, ForceMode.Impulse);
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
    public void SetMaterial(Material skinMaterial, Material faceMaterial)
    {
        this.skinMaterial = skinMaterial;
        this.faceMaterial = faceMaterial;
        Material[] mat = new Material[] { skinMaterial, faceMaterial };
        playerMesh.GetComponent<SkinnedMeshRenderer>().materials = mat;
    }
    public void ChangeCustomizing(int itemCode)
    {
        ItemEntity tItem = Managers.Data.GetItemData(itemCode);
        switch(tItem.Item_Type)
        {
            //Skin
            case 10:
                {
                    skinMaterial = Resources.Load<Material>($"Materials/{tItem.File_Name}");
                    SetMaterial(skinMaterial, faceMaterial);
                }
                break;
            //Head Item
            case 21:
                {
                    if(headItem != null)
                    {
                        headItem.SetActive(false);
                    }
                    for (int i = 0; i < Head_Equipments.transform.childCount; ++i)
                    {
                        if (Head_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            headItem = Head_Equipments.transform.GetChild(i).gameObject;
                            headItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Face Item
            case 22:
                {
                    if (faceItem != null)
                    {
                        faceItem.SetActive(false);
                    }
                    for (int i = 0; i < Face_Equipments.transform.childCount; ++i)
                    {
                        if (Face_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            faceItem = Face_Equipments.transform.GetChild(i).gameObject;
                            faceItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Back Item
            case 23:
                {
                    if (backItem != null)
                    {
                        backItem.SetActive(false);
                    }
                    for (int i = 0; i < Back_Equipments.transform.childCount; ++i)
                    {
                        if (Back_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            backItem = Back_Equipments.transform.GetChild(i).gameObject;
                            backItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Face
            case 24:
                {
                    faceMaterial = Resources.Load<Material>($"Materials/{tItem.File_Name}");
                    SetMaterial(skinMaterial, faceMaterial);
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! ChangeCustomizing(): Wrong Item Type!!!");
                }
                break;
        }
    }
    private void HideItem()
    {
        if (headItem != null) headItem.SetActive(false);
        if (faceItem != null) faceItem.SetActive(false);
        if (backItem != null) backItem.SetActive(false);
    }
    private void ShowItem()
    {
        if (headItem != null) headItem.SetActive(true);
        if (faceItem != null) faceItem.SetActive(true);
        if (backItem != null) backItem.SetActive(true);
    }

    // ------------------- UI --------------------
    public void SetUINowCamera(Camera targetCamera)
    {
        playerUIController.SetCamera(targetCamera);
    }
    public void SetUITeam()
    {
        playerUIController.gameObject.transform.GetChild(teamNumber + 1).gameObject.SetActive(true);
    }
    public void SetUIMe()
    {
        if (amIPlayer == true)
        {
            playerUIController.gameObject.transform.GetChild(0).gameObject.SetActive(true);
        }
    }
}