using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ClientProtocol;

public class CharacterStatus : MonoBehaviour
{
    [Header("--- Status ---")]
    [SerializeField]
    private int id;
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
    [SerializeField]
    private bool isHaveItem = false;
    private string itemTag;
    private int itemId;
    public GameObject playerMesh;

    [Header("--- AnimationControll ---")]
    public bool isGroggy;
    public AnimationController animationController;
    private ActiveRagdoll.AnimationModule animationMoudule;
    // 상체의 상태는 서버와 교환
    private UpperBodyAnimationState upperBodyAnimationState;
    // 하체의 상태는 클라가 관리(이동 관련해서는 이미 서버와 교환하고 있기 때문)
    private LowerBodyAnimationState lowerBodyAnimationState;

    private PlayerController playerController;
    public Rigidbody headRig;
    public float attackedStrength = 10;

    //------ Server -------
    private NetworkManager network;
    private PacketManager packetManager;

    // 레이어 관련
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
            // 로딩 창 없애기
            Managers.UI.ClosePopUpUI(GameObject.FindObjectOfType<UI_Loading>());

            // 씬 기본 UI 표시해주기
            Managers.UI.ShowSceneUI<UI_OneVersusOneSceneUI>();

            // 타이머 스타트
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
        if (this.upperBodyAnimationState != upperBodyAnimationState)
        {
            ///<summary>
            ///서버에 상태 전달하는 부분 여기에 추가
            ///</summary>
            packetManager.SendPlayerAnimationPacket(playerController.GetPosition(), playerController.GetDirection(), id, upperBodyAnimationState);

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
    public void SetIsHaveItem(bool isHaveItem, string itemTag = "NULL", int itemId = -1)
    {
        this.isHaveItem = isHaveItem;
        this.itemTag = itemTag;
        this.itemId = itemId;
    }
    public bool GetIsHaveItem()
    {
        return isHaveItem;
    }
    public string GetItemTag()
    {
        return itemTag;
    }
    public int GetItemId()
    {
        return itemId;
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
        // 플레이어 그로기 및 죽음 처리 전까지 확인용 로그
        Debug.Log(this.name + " is Die? : " + isDie);
    }
    public bool GetIsDie()
    {
        return isDie;
    }
    public void SetId(int id)
    {
        this.id = id;
    }
    public int GetId()
    {
        return id;
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
        SetIsHaveItem(false);

        playerController.ResetPlayerControllerSetting();
    }
}