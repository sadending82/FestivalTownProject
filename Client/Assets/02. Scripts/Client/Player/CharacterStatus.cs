using ClientProtocol;
using UnityEngine;

public class CharacterStatus : MonoBehaviour
{
    [Header("--- Status ---")]
    [SerializeField]
    private int Id;
    public int maxHp;
    public int hp;
    public float maxStamina;
    public float stamina;
    public float staminaRecoverySpeed;
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

    // �Ⱦ�����
    [Header("--- PickUp ---")]
    [SerializeField]
    private bool isHaveBomb = false;
    private int bombId;
    [SerializeField]
    private bool isHaveWeapon = false;
    private int weaponId;
    public GameObject weaponInven;


    [Header("--- AnimationControll ---")]
    public bool isGroggy;
    public AnimationController animationController;
    private ActiveRagdoll.AnimationModule animationMoudule;
    // ��ü�� ���´� ������ ��ȯ
    private UpperBodyAnimationState upperBodyAnimationState;
    // ��ü�� ���´� Ŭ�� ����(�̵� �����ؼ��� �̹� ������ ��ȯ�ϰ� �ֱ� ����)
    private LowerBodyAnimationState lowerBodyAnimationState;

    private PlayerController playerController;
    public Rigidbody headRig;
    public float attackedStrength = 10;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;

    // ���̾� ����
    [Header("--- LayerControl ---")]
    public GameObject pelvis;
    public GameObject Hitbox;

    private void Awake()
    {
        amIPlayer = false;
        myCamera.enabled = false;
    }
    private void Start()
    {
        network = Managers.Network;
        packetManager = network.GetPacketManager();

        playerController = GetComponent<PlayerController>();
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
            // �ε� â ���ֱ�
            Managers.UI.ClosePopUpUI(GameObject.FindObjectOfType<UI_Loading>());

            // �� �⺻ UI ǥ�����ֱ�
            Managers.UI.ShowSceneUI<UI_OneVersusOneSceneUI>();

            myCamera.gameObject.GetComponent<AudioListener>().enabled = true;

            // Ÿ�̸� ��ŸƮ
            Managers.Game.isTimerStart = true;
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
        Hitbox.layer = layer;

        ChangeLayerRecursively(pelvis, layer);
    }
    // ��� �ڽ� ������Ʈ�� ������ ���̾ �ٲٴ� �Լ�
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
        if (this.upperBodyAnimationState != upperBodyAnimationState)
        {
            ///<summary>
            ///������ ���� �����ϴ� �κ� ���⿡ �߰�
            ///</summary>
            packetManager.SendPlayerAnimationPacket(playerController.GetPosition(), playerController.GetDirection(), Id, upperBodyAnimationState);

            this.upperBodyAnimationState = upperBodyAnimationState;
            animationController.SetUpperBodyAnimationState(upperBodyAnimationState);
        }
    }
    public void s_SetUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        if (this.upperBodyAnimationState != upperBodyAnimationState)
        {
            this.upperBodyAnimationState = upperBodyAnimationState;
            animationController.SetUpperBodyAnimationState(upperBodyAnimationState);
        }
    }

    public void SetLowerBodyAnimationState(LowerBodyAnimationState lowerBodyAnimationState)
    {
        if (this.lowerBodyAnimationState != lowerBodyAnimationState)
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

        playerController.ResetPlayerControllerSetting();
    }

    public GameObject GetWeaponInven()
    {
        return weaponInven;
    }
}