using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Animations;

public class Weapon : MonoBehaviour
{
    // ------ Server -------
    [SerializeField]
    private int Id;

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
    }
    private void OnEnable()
    {
        isPickUp = false;
        pickUpPlayerId = -1;
    }

    public void PickUp(int pickUpPlayerId)
    {
        isPickUp = true;
        this.pickUpPlayerId = pickUpPlayerId;
        this.lastPickUpPlayerId = pickUpPlayerId;

        GameObject pickUpPlayer = Managers.Player.FindPlayerById(pickUpPlayerId);
        GameObject weaponInven = pickUpPlayer.GetComponent<CharacterStatus>().GetWeaponInven();
        // ���̾� PickUpPlayer�� ����
        this.gameObject.layer = weaponInven.layer;

        // Parent Constraint ���� - ���Ⱑ �տ� �ٰ�
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

        // Parent Constraint ���� - ���Ⱑ �տ��� ��������
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
        if(isPickUp == false)
        {
            Debug.Log("Error!!! DeleteWeapon(), Cant Delete Weapon !!!");
        }
        else
        {
            Debug.Log("Im Deleted : " + Id);
            Managers.Resource.Destroy(this.gameObject);
        }
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
