using ClientProtocol;
using UnityEngine;

public class CharacterStatus : MonoBehaviour
{
    [Header("--- Status ---")]
    [SerializeField]
    private int Id;
    public int maxHp;
    public int hp;
    public int maxStamina;
    public int stamina;
    public int staminaRecoverySpeed;
    public int strength;
    public int speed;
    public float attackSpeed;
    public float runSpeedRatio;
    private float rollSpeed;
    public bool isDie = false;
    public int groggyNum;
    public float groggyTime;
    public Camera myCamera;
    private bool amIPlayer;
    public GameObject playerMesh;
    public int teamNumber;

    // 픽업관련
    [Header("--- PickUp ---")]
    [SerializeField]
    private bool isHaveBomb = false;
    private int bombId;
    [SerializeField]
    private bool isHaveWeapon = false;
    private int weaponId;
    public GameObject weaponInven;
    private GameObject myWeapon;
    [SerializeField]
    private bool isGrapPlayer = false;
    private int grapTargetPlayerId;


    [Header("--- AnimationControll ---")]
    private bool isGroggy;
    public AnimationController animationController;
    public ActiveRagdoll.AnimationModule animationMoudule;
    // 상체의 상태는 서버와 교환
    private UpperBodyAnimationState upperBodyAnimationState;
    // 하체의 상태는 클라가 관리(이동 관련해서는 이미 서버와 교환하고 있기 때문)
    private LowerBodyAnimationState lowerBodyAnimationState;

    public PlayerController playerController;
    public Rigidbody headRig;
    public float attackedStrength = 10;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;

    // 레이어 관련
    [Header("--- LayerControl ---")]
    public GameObject pelvis;
    public GameObject hitbox;

    private void Awake()
    {
        amIPlayer = false;
        myCamera.enabled = false;

        maxStamina = 100;
        stamina = 100;
        staminaRecoverySpeed = 5;

        if(animationMoudule == null)
        {
            animationMoudule = GetComponent<ActiveRagdoll.AnimationModule>();
        }
    }

    private void Start()
    {
        network = Managers.Network;
        packetManager = network.GetPacketManager();
    }
    private void Update()
    {
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
        this.GetComponent<PlayerController>().SetAmIPlayer(amIPlayer);
        this.GetComponent<PlayerCameraController>().SetAmIPlayer(amIPlayer);
        if(amIPlayer == true)
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

    public void SetLayer(int id)
    {
        string layerName = "Player" + id;
        int layer = LayerMask.NameToLayer(layerName);
        hitbox.layer = layer;

        ChangeLayerRecursively(pelvis, layer);
    }
    // 모든 자식 오브젝트에 접근해 레이어를 바꾸는 함수
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
            ///<summary>
            ///서버에 상태 전달하는 부분 여기에 추가
            ///</summary>
            packetManager.SendPlayerAnimationPacket(playerController.GetPosition(), playerController.GetDirection(), Id, upperBodyAnimationState);

            this.upperBodyAnimationState = upperBodyAnimationState;
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
    public UpperBodyAnimationState GetUpperBodyAnimationState()
    {
        return this.upperBodyAnimationState;
    }
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

    public void SetIsDie(bool isDie)
    {
        this.isDie = isDie;
        if(isDie == true)
        {
            hp = 0;
            playerMesh.SetActive(false);
            Managers.Sound.Play("Sfx_Ch_Die");
        }
        else
        {
            ResetCharacterState();
            playerMesh.SetActive(true);
        }
    }
    public bool GetIsDie()
    {
        return isDie;
    }
    public void SetId(int Id)
    {
        this.Id = Id;
    }
    public int GetId()
    {
        return Id;
    }
    public int GetStrength()
    {
        return strength;
    }
    public float GetAttackSpeed()
    {
        return attackSpeed;
    }

    public void Attacked(Vector3 direction)
    {
        headRig.AddForce(direction * attackedStrength, ForceMode.Impulse);
    }
    public void SetHp(int hp)
    {
        this.hp = hp;
    }

    public void ResetCharacterState()
    {
        SetUpperBodyAnimationState(UpperBodyAnimationState.NONE);
        SetLowerBodyAnimationState(LowerBodyAnimationState.IDLE);
        SetIsHaveBomb(false);
        SetIsHaveWeapon(false);
        GroggyOff();

        playerController.ResetPlayerControllerSetting();
    }

    public GameObject GetWeaponInven()
    {
        return weaponInven;
    }

    public GameObject GetMyWeapon()
    {
        return myWeapon;
    }

    public Vector3 GetAttackedDirection(Vector3 attackerPosition)
    {
        Vector3 attackedDirection = attackerPosition - pelvis.transform.position;
        attackedDirection.y = 0;

        return attackedDirection.normalized;
    }
    public void CameraOn()
    {
        myCamera.enabled = true;
    }
    public void CameraOff()
    {
        myCamera.enabled = false;
    }
    public void SetTeamNumber(int teamNumber)
    {
        this.teamNumber = teamNumber;
    }
    public int GetTeamNumber()
    {
        return teamNumber;
    }

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
            isGroggy = false;
            SetStamina(maxStamina);
            animationMoudule.GroggyOff();
        }
    }
    public bool GetIsGroggy()
    {
        return isGroggy;
    }
    public void SetStamina(int stamina)
    {
        this.stamina = stamina;
    }
    public int GetStamina()
    {
        return stamina;
    }
    public Rigidbody GetHeadRigidbody()
    {
        return headRig;
    }
    public void SetIsGrapPlayer(bool isGrapPlayer, int targetPlayerId = -1)
    {
        this.isGrapPlayer = isGrapPlayer;
        this.grapTargetPlayerId = targetPlayerId;
    }
    public int GetGrapTargetPlayerId()
    {
        return grapTargetPlayerId;
    }
}