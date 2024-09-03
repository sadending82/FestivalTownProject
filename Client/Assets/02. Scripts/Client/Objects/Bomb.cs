using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    public GameObject bombPusher;

    private bool isPickUp = false;
    private int pickUpPlayerId;
    private int lastPickUpPlayerId;
    private Transform targetTransform;

    private Vector3 throwDirection;
    private Rigidbody rig;

    // ------ Server -------
    [SerializeField]
    private int id;

    private void Awake()
    {
        rig = this.GetComponent<Rigidbody>();
    }
    private void OnEnable()
    {
        isPickUp = false;
        pickUpPlayerId = -1;
    }
    private void OnDisable()
    {
        SetRigidBodyBasic();
    }
    private void FixedUpdate()
    {
        if (isPickUp == true)
        {
            this.transform.position = targetTransform.position;
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        // �ٴڿ� ���������� ���̻� �������� �ʵ��� ����
        if (collision.gameObject.tag == "Ground" && isPickUp == false)
        {
            rig.constraints = RigidbodyConstraints.FreezePosition |
                              RigidbodyConstraints.FreezeRotation;
        }
    }
    public void Boom()
    {
        GameObject pusher = Managers.Resource.Instantiate("BombPusher");

        pusher.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, this.transform.position.z);
        pusher.SetActive(true);

        if(isPickUp == true)
        {
            /// <summary>
            ///  ���� �Ŵ��� �߰� �Ǹ� ���⼭ ���� �Ŵ������� ���� �÷��̾� ���̵� ������
            /// </summary>
            Debug.Log("Player " + pickUpPlayerId + " is Die");

            PickUpOff();
        }
        rig.velocity = Vector3.zero;

        Managers.Resource.Destroy(this.gameObject);
    }
    public bool GetIsPickUp()
    {
        return isPickUp;
    }

    public void PickUp(int pickUpPlayerId, Transform targetTransform)
    {
        isPickUp = true;
        this.pickUpPlayerId = pickUpPlayerId;
        this.lastPickUpPlayerId = pickUpPlayerId;
        this.targetTransform = targetTransform;
        SetRigidBodyPickUp();
    }
    public void PickUpOff()
    {
        CharacterStatus pickUpPlayerStatus = Managers.Player.transform.GetChild(pickUpPlayerId).GetComponent<CharacterStatus>();
        pickUpPlayerStatus.SetIsHaveItem(false);

        isPickUp = false;
        this.pickUpPlayerId = -1;
        this.targetTransform = null;
    }

    public void Throw(Vector3 throwDirection, int throwStrength)
    {
        this.throwDirection = throwDirection;
        SetRigidBodyBasic();

        rig.AddForce(throwDirection * throwStrength, ForceMode.Impulse);

        PickUpOff();
    }

    private void SetRigidBodyPickUp()
    {
        rig.constraints = RigidbodyConstraints.FreezeRotation;
        rig.useGravity = false;
        rig.mass = 0;
        rig.angularDrag = 0;
    }
    private void SetRigidBodyBasic()
    {
        rig.constraints = RigidbodyConstraints.FreezeRotation;
        rig.useGravity = true;
        rig.mass = 1;
        rig.angularDrag = 0.05f;
    }
    public void SetPosition(Vector3 position)
    {
        transform.position = position;
    }
    public void SetId(int id)
    {
        this.id = id;
    }
    public int GetId()
    {
        return id;
    }
    public int GetLastPickUpPlayerId()
    {
        return lastPickUpPlayerId;
    }
}
