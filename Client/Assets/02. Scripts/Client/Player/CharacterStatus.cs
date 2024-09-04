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
        // �÷��̾� �׷α� �� ���� ó�� ������ Ȯ�ο� �α�
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