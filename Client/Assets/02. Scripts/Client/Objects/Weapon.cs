using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Animations;

public class Weapon : MonoBehaviour
{
    // ------ Server -------
    [SerializeField]
    private int Id;
    private PacketManager packetManager;

    // ------ Client -------
    private bool isPickUp = false;
    private int pickUpPlayerId;
    private int lastPickUpPlayerId;

    private ParentConstraint parentConstraint;
    private ConstraintSource weaponInvenSource;

    private Transform basicTransform;

    private void Awake()
    {
        basicTransform = transform;
    }
    private void Start()
    {
        parentConstraint = GetComponent<ParentConstraint>();
        packetManager = Managers.Network.GetPacketManager();
    }
    private void OnEnable()
    {
        isPickUp = false;
        pickUpPlayerId = -1;
    }
    private void FixedUpdate()
    {
        if(Managers.Player.GetIsHost() == true && this.transform.position.y <= -10f)
        {
            packetManager.SendWeaponDeletePacket(Id);
        }
    }

    public void PickUp(int pickUpPlayerId)
    {
        isPickUp = true;
        this.pickUpPlayerId = pickUpPlayerId;
        this.lastPickUpPlayerId = pickUpPlayerId;

        GameObject pickUpPlayer = Managers.Player.FindPlayerById(pickUpPlayerId);
        GameObject weaponInven = pickUpPlayer.GetComponent<CharacterStatus>().GetWeaponInven();
        // 레이어 PickUpPlayer랑 통일
        this.gameObject.layer = weaponInven.layer;

        // Parent Constraint 설정 - 무기가 손에 붙게
        weaponInvenSource.sourceTransform = weaponInven.transform;
        weaponInvenSource.weight = 1;
        parentConstraint.SetSource(0, weaponInvenSource);
        parentConstraint.constraintActive = true;
    }

    public void Drop(Vector3 position)
    {
        isPickUp = false;
        this.pickUpPlayerId = -1;
        this.gameObject.layer = LayerMask.NameToLayer("Weapon");

        // Parent Constraint 해제 - 무기가 손에서 떨어지게
        weaponInvenSource.sourceTransform = null;
        weaponInvenSource.weight = 1;
        parentConstraint.SetSource(0, weaponInvenSource);
        parentConstraint.constraintActive = false;

        this.transform.rotation = basicTransform.rotation;
        this.transform.position = position;

        CharacterStatus ps = Managers.Player.FindPlayerById(lastPickUpPlayerId).GetComponent<CharacterStatus>();
        ps.SetIsHaveWeapon(false);
    }

    public void DeleteWeapon()
    {
        if(isPickUp == true)
        {
            Drop(transform.position);
        }
        Managers.Resource.Destroy(this.gameObject);
    }

    public void SetId(int Id)
    {
        this.Id = Id;
    }
    public int GetId()
    {
        return Id;
    }
    public bool GetIsPickUp()
    {
        return isPickUp;
    }
}
